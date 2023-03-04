#include "shooter.hpp"
#include "flywheel.hpp"
#include "intake.hpp"
#include "../Libraries/logging.hpp"
#include "pros/misc.hpp"

const int toaster_rpm = 1460;
const int barrier_rpm = 1800;// 2380 For long shots, 1775 for short shots, 2125 for middle shots
// const int barrier_rpm = 2235;

bool goal_disturb = false;

Timer ShooterShootParams::shoot_timer{"shoot_timer"};
Timer ShooterShootParams::disc_seen_timer{"disc_seen_timer"};
Timer ShooterShootParams::disc_absence_timer{"disc_absence_timer"};

// 1820
bool angleOverride = false;

Machine<SHOOTER_STATE_TYPES> shooter("shooter", ShooterIdleParams{});

void shooterHandleInput(){
  SHOOTER_STATE_TYPES_VARIANT cur_state = shooter.getState();
  if(get_if<ShooterIdleParams>(&cur_state)){
    if(master.get_digital_new_press(tripleShotBtn)) shoot(3);
    if(master.get_digital_new_press(singleShotBtn)) shoot(1);
  }

  if(master.get_digital_new_press(anglerToggleBtn)) {
    angler_p.toggleState();
    handleRpm();
  } 

  // if (master.get_digital_new_press(angleOverrideBtn)) {
  //   angleOverride = !angleOverride; 
  //   handleRpm();
  // }

  // if(master.get_digital_new_press(goalDisturbBtn)){
  //   goal_disturb = !goal_disturb;
  //   if(goal_disturb && angler_p.getState() == 1)  setFlywheelVel(3600);
  // }
  
  
}

// Shooter idle state

const char* ShooterIdleParams::getName(){
  return "ShooterIdle";
}
void ShooterIdleParams::handle(){}
void ShooterIdleParams::handleStateChange(SHOOTER_STATE_TYPES_VARIANT prev_state){}

// Shooter shoot state

ShooterShootParams::ShooterShootParams(int shots, bool match_load): shots_left(shots), match_load(match_load){}

const char* ShooterShootParams::getName(){
  return "ShooterShoot";
}
void ShooterShootParams::handle(){
  // log("Shoot_time: %lld \n", shoot_timer.getTime());
  // Fires shot if flywheel rpm is within 20 of target and 300 ms has elapsed
  if(goal_disturb){
    if(fabs(flywheel_error) > 1000) cycle_check.reset();
  }
  else{
    FLYWHEEL_STATE_TYPES_VARIANT temp_flywheel_state = flywheel.getState();
    if(get_if<FlywheelMoveVelParams>(&temp_flywheel_state)->target_vel > 2000){
      if(fabs(flywheel_error) > 30) cycle_check.reset();
    }
    else if(fabs(flywheel_error) > 150)  cycle_check.reset();
  }

  // log("cycle_check:%lld\n", cycle_check.getTime());
  // cycle_check.getTime() >= 30
  // flywheel_error.load() < 20

  disc_seen = mag_ds.get_value() < 1000;
  if(disc_seen && !disc_seen_last){ // Just saw disc
    log("%d JUST SAW mag: %d\n", millis(), mag_ds.get_value());

    disc_seen_timer.reset();
    disc_absence_timer.reset(false);
  }
  else if(disc_seen_last && !disc_seen){  // Just saw lack of discs
    log("%d JUST LOST DISK mag: %d\n", millis(), mag_ds.get_value());

    disc_seen_timer.reset(false);
    disc_absence_timer.reset();
  }
  disc_seen_last = disc_seen;


  bool trigger = shoot_timer.getTime() > 250; // Doesn't wait for flywheel because we want Robert to shoot no matter what
  // trigger = shoot_timer.getTime() > 400 && cycle_check.getTime() >= 50;

  log("%d mag: %d\n", millis(), mag_ds.get_value());

  if (angler_p.getState() == HIGH){
    trigger = shoot_timer.getTime() > 250; // Doesn't wait for flywheel because we want Robert to shoot no matter what
    // trigger = shoot_timer.getTime() > 400 && cycle_check.getTime() >= 30;
  } 
  else if(pros::competition::is_autonomous()){
    trigger = shoot_timer.getTime() > 400 && cycle_check.getTime() >= 50;
  }
  
  if(match_load) trigger = shoot_timer.getTime() > 250 && cycle_check.getTime() >= 10 && disc_seen_timer.getTime() > 100;
  
  if(trigger){ // && cycle_check.getTime() >= 30){
    log("%d STARTED SHOOTING\n", millis());
    shoot_timer.reset();
    indexer_p.setState(HIGH);	
    _Task::delay(100); // Waits for SHOOTER to extend
    log(" %d FINISHED SHOT\n", millis());
    indexer_p.setState(LOW);
    log("%d FINISHED Retraction\n", millis());
    shots_left--;
    log("shots left: %d\n", shots_left);
    if (g_mag_disc_count > 0) g_mag_disc_count--;
    

    _Task::delay(75);// wait for SHOOTER to retract // DON'T CHANGE THIS LINE 

    if(shots_left <= 0){  // If shooting is done
      master.rumble("-"); // Lets driver know shooting is done
      log("CONTROLLER RUMBLING FROM LINE 126 in file shooter.cpp");
      g_mag_disc_count = 0;
      _Task::delay(150); // Waits for last disc to shoot
      // Sets subsystems back to their state before shooting
      intakeOn();
      shooter.changeState(ShooterIdleParams{}, 109);

    }

  }
  // Ends shooting if disc hasn't been seen for 2 seconds
  if(match_load && disc_absence_timer.getTime() > 2000){
    master.rumble("-"); // Lets driver know shooting is done
    log("CONTROLLER RUMBLING FROM LINE 140 in file shooter.cpp");
    log("FINISHED MATCH LOADER, TIMED OUT\n");

    g_mag_disc_count = 0;
    _Task::delay(150); // Waits for last disc to shoot
    // Sets subsystems back to their state before shooting
    intakeOn();
    shooter.changeState(ShooterIdleParams{}, 109);

    handleRpm();

  }

}
void ShooterShootParams::handleStateChange(SHOOTER_STATE_TYPES_VARIANT prev_state){
  // angler_p.setState(LOW);
  log("INIT shots_left %d\n", shots_left);
  intakeIndex();  // Sets intake to index state
  FLYWHEEL_STATE_TYPES_VARIANT temp_flywheel_state = flywheel.getState();

  disc_seen_timer.reset(false);
  disc_absence_timer.reset();

  // if(get_if<FlywheelMoveVelParams>(&temp_flywheel_state)->target_vel > 2000) get_if<FlywheelMoveVelParams>(&temp_flywheel_state)->kP = 0.7;
  // else get_if<FlywheelMoveVelParams>(&temp_flywheel_state)->kP = 0.5;

}

void shoot(int shots, bool match_load){
  log("Shot requested for %d shots at %d\n", millis(), shots);
  shooter.changeState(ShooterShootParams{shots, match_load});
}

void handleRpm() {
  if (goal_disturb) setFlywheelVel(3600); // Goal_disturb
  else if (!pros::competition::is_autonomous()) { // Automatic
    if (angler_p.getState() == LOW) setFlywheelVel(barrier_rpm);
    else setFlywheelVel(toaster_rpm);
  }
}