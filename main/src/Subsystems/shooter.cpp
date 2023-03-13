#include "shooter.hpp"
#include "flywheel.hpp"
#include "intake.hpp"
#include "../config.hpp"
#include "../drive.hpp"
#include "../Devices/controller.hpp"
#include "../Devices/piston.hpp"
#include "../Devices/others.hpp"

Timer ShooterShootParams::shoot_timer{"Shot", subsystem_log};
Timer ShooterShootParams::disc_seen_timer{"Disc Seen"};
Timer ShooterShootParams::disc_absence_timer{"Disc Absent"};
Timer ShooterShootParams::cycle_check{"Cycle Check", subsystem_log};

Machine<SHOOTER_STATE_TYPES> shooter{"Shooter", ShooterIdleParams{}};
static constexpr int toaster_rpm = 1425;
bool angleOverride = false;
bool goal_disturb = false;

void shooterHandleInput(){
  shooterVariant cur_state = shooter.getState();
  if(std::get_if<ShooterIdleParams>(&cur_state)){
    if(master.getNewDigital(tripleShotBtn)){
      subsystem_log("%lld | 3 BUTTON PRESSED", op_control_timer.getTime());
  
      shoot(3, false, true);
    }
    if(master.getNewDigital(singleShotBtn)){
      subsystem_log("%lld | 1 BUTTON PRESSED", op_control_timer.getTime());
      shoot(1);
    }
  }

  if(master.getNewDigital(anglerToggleBtn)) {
    subsystem_log("%lld | ANGLER TOGGLE BUTTON PRESSED", op_control_timer.getTime());
    angler_p.toggleState();
    if(angler_p.getState()) intakeOff();
    else if(g_mag_disc_count < 3) intakeOn();
    handleRpm();
  } 

  // if (master.getNewDigital(angleOverrideBtn)) {
  //   angleOverride = !angleOverride; 
  //   handleRpm();
  // }

  // if(master.getNewDigital(goalDisturbBtn)){
  //   goal_disturb = !goal_disturb;
  //   if(goal_disturb && angler_p.getState() == 1) setFlywheelVel(3600);
  // }
}

// Shooter idle state

void ShooterIdleParams::handle(){}
void ShooterIdleParams::handleStateChange(shooterVariant prev_state){}

// Shooter shoot state

ShooterShootParams::ShooterShootParams(int shots, bool match_load, bool clear_mag): shots(shots), shots_left(shots), match_load(match_load), clear_mag(clear_mag){}

void ShooterShootParams::handle(){


  // subsystem_log("Shoot_time: %lld \n", shoot_timer.getTime());
  // Fires shot if flywheel rpm is within 20 of target and 300 ms has elapsed
  if(goal_disturb){
    if(std::abs(flywheel_error) > 1000) cycle_check.reset();
  }
  else{
    flywheelVariant temp_flywheel_state = flywheel.getState();
    if(std::get_if<FlywheelMoveVelParams>(&temp_flywheel_state) && std::get_if<FlywheelMoveVelParams>(&temp_flywheel_state)->target_vel > barrier_rpm){
      if(std::abs(flywheel_error) > 30) cycle_check.reset();
    }
    else if(std::abs(flywheel_error) > 150)  cycle_check.reset();
  }

  // cycle_check.print();
  // cycle_check.getTime() >= 30
  // flywheel_error.load() < 20

  disc_seen = mag_ds.getVal() < 1000;
  if(disc_seen && !disc_seen_last){ // Just saw disc
    subsystem_log("%d JUST SAW mag: %d", millis(), mag_ds.getVal());

    disc_seen_timer.reset();
    disc_absence_timer.reset(false);
  }
  else if(disc_seen_last && !disc_seen){  // Just saw lack of discs
    subsystem_log("%d JUST LOST DISK mag: %d", millis(), mag_ds.getVal());

    disc_seen_timer.reset(false);
    disc_absence_timer.reset();
  }
  disc_seen_last = disc_seen;


  bool trigger = shoot_timer.getTime() > 250; // Doesn't wait for flywheel because we want driver to shoot no matter what
  // trigger = shoot_timer.getTime() > 400 && cycle_check.getTime() >= 50;

  //subsystem_log("%d mag: %d", millis(), mag_ds.getVal());

  if (angler_p.getState() == HIGH){
    trigger = shoot_timer.getTime() > 250; // Doesn't wait for flywheel because we want driver to shoot no matter what
    // trigger = shoot_timer.getTime() > 400 && cycle_check.getTime() >= 30;
  }
  else if(competition::is_autonomous()){
    trigger = shoot_timer.getTime() > 400 && cycle_check.getTime() >= 50;
  }
  
  // Takes match load shot if disc settles in mag for 100ms
  if(match_load) trigger = shoot_timer.getTime() > 250 && cycle_check.getTime() >= 10 && disc_seen_timer.getTime() > 100;
  
  if(trigger){ // && cycle_check.getTime() >= 30){
    subsystem_log("%d STARTED SHOOTING\n", millis());
    shoot_timer.reset();
    indexer_p.setState(HIGH);
    _Task::delay(100); // Waits for SHOOTER to extend
    subsystem_log("%d FINISHED SHOT", millis());
    indexer_p.setState(LOW);
    subsystem_log("%d FINISHED Retraction", millis());
    shots_left--;
    subsystem_log("shots left: %d", shots_left);
    if (g_mag_disc_count > 0) g_mag_disc_count--;

    _Task::delay(75); // wait for SHOOTER to retract

    if((!clear_mag && shots_left <= 0) || shots_left <= -2){  // If shooting is done
      master.rumble(); // Lets driver know shooting is done
      //subsystem_log("CONTROLLER RUMBLING FROM LINE %d in file %s", __LINE__, __FILE__);
      if(clear_mag) g_mag_disc_count = 0;
      else{

        g_mag_disc_count -= shots;
        subsystem_log("SHO`````TS:%d\n", shots);
      }
      _Task::delay(150); // Waits for last disc to shoot
      // Sets subsystems back to their state before shooting
      intakeOn();
      shooter.changeState(ShooterIdleParams{});

    }
  }
  // Ends shooting if disc hasn't been seen for 2 seconds
  if((match_load && disc_absence_timer.getTime() > 2000) || (shots_left <= 0 && clear_mag && disc_absence_timer.getTime() > 100)){
    master.rumble("-"); // Lets driver know shooting is done
    //subsystem_log("CONTROLLER RUMBLING FROM LINE %d in file %s", __LINE__, __FILE__);
    subsystem_log("FINISHED MATCH LOADER, TIMED OUT");

    if(clear_mag) g_mag_disc_count = 0;
    else  g_mag_disc_count -= shots;
    // Sets subsystems back to their state before shooting
    intakeOn();
    shooter.changeState(ShooterIdleParams{});

    handleRpm();
  }

}

void ShooterShootParams::handleStateChange(shooterVariant prev_state){
  // angler_p.setState(LOW);
  subsystem_log("INIT shots_left %d", shots_left);
  intakeIndex();  // Sets intake to index state
  flywheelVariant temp_flywheel_state = flywheel.getState();

  disc_seen_timer.reset(false);
  disc_absence_timer.reset();

  if(match_load)  clear_mag = true; // All match load types shots should also clear any extra discs

  // if(std::get_if<FlywheelMoveVelParams>(&temp_flywheel_state)->target_vel > 2000) std::get_if<FlywheelMoveVelParams>(&temp_flywheel_state)->kP = 0.7;
  // else std::get_if<FlywheelMoveVelParams>(&temp_flywheel_state)->kP = 0.5;
}

void shoot(int shots, bool match_load, bool clear_mag){
  subsystem_log("Shot requested for %d shots at %d", millis(), shots);
  shooter.changeState(ShooterShootParams{shots, match_load, clear_mag});
}

void handleRpm() {
  if (goal_disturb) setFlywheelVel(3600); // Goal_disturb
  else if (!pros::competition::is_autonomous()) { // Automatic
    if (angler_p.getState() == LOW) setFlywheelVel(barrier_rpm);
    else setFlywheelVel(toaster_rpm);
  }
}