#include "shooter.hpp"
#include "flywheel.hpp"
#include "intake.hpp"
#include "../Libraries/logging.hpp"
#include "pros/misc.hpp"

const int toaster_rpm = 1450;
const int barrier_rpm = 1875;
// const int barrier_rpm = 2235;

bool goal_disturb = false;

Timer ShooterShootParams::shoot_timer{"shoot_timer"};

// 1820
bool angleOverride = false;

Machine<SHOOTER_STATE_TYPES> shooter("shooter", ShooterIdleParams{});

void shooterHandleInput(){
  SHOOTER_STATE_TYPES_VARIANT cur_state = shooter.getState();
  if(get_if<ShooterIdleParams>(&cur_state)){
    if(master.get_digital_new_press(tripleShotBtn)) shoot(3);
    if(master.get_digital_new_press(singleShotBtn)) shoot(2);
  }

  if(master.get_digital_new_press(anglerToggleBtn)) {
    angler_p.toggleState();
    if (!angleOverride){
      if (angler_p.getState() == 0) setFlywheelVel(barrier_rpm);
      else{
        if(goal_disturb)  setFlywheelVel(3600);
        else  setFlywheelVel(toaster_rpm);
      }
    }
  } 

  if (master.get_digital_new_press(angleOverrideBtn)) {
    angleOverride = !angleOverride; 
    if(angleOverride) {
      setFlywheelVel(toaster_rpm);
    }
    else{
      if (angler_p.getState() == 0) setFlywheelVel(barrier_rpm);
      else{
        if(goal_disturb)  setFlywheelVel(3600);
        else  setFlywheelVel(toaster_rpm);
      }
    }
  }

  if(master.get_digital_new_press(goalDisturbBtn)){
    goal_disturb = !goal_disturb;
    if(goal_disturb && angler_p.getState() == 1)  setFlywheelVel(3600);
  }
  
  
}

// Shooter idle state

const char* ShooterIdleParams::getName(){
  return "ShooterIdle";
}
void ShooterIdleParams::handle(){}
void ShooterIdleParams::handleStateChange(SHOOTER_STATE_TYPES_VARIANT prev_state){}

// Shooter shoot state

ShooterShootParams::ShooterShootParams(int shots): shots_left(shots){}

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
      if(fabs(flywheel_error) > 150) cycle_check.reset();
    }
    else if(fabs(flywheel_error) > 150)  cycle_check.reset();
  }

  // log("cycle_check:%lld\n", cycle_check.getTime());
  // cycle_check.getTime() >= 30
  // flywheel_error.load() < 20

  bool trigger = shoot_timer.getTime() > 350 && cycle_check.getTime() >= 30;

  if (angler_p.getState() == HIGH){
    trigger = shoot_timer.getTime() > 250 && cycle_check.getTime() >= 30;
    // trigger = shoot_timer.getTime() > 400 && cycle_check.getTime() >= 30;

  } else if(pros::competition::is_autonomous()){
    trigger = shoot_timer.getTime() > 400 && cycle_check.getTime() >= 30;
  }
  // else if (check if shootig from far){
  //   trigger = // ...
  // }
  
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
      g_mag_disc_count = 0;
      _Task::delay(100); // waits for last disc to shoot
      // Sets subsystems back to their state before shooting
      intakeOn();
      shooter.changeState(ShooterIdleParams{}, 102);

      if (!angleOverride && !pros::competition::is_autonomous()){
        if (angler_p.getState()==0) setFlywheelVel(barrier_rpm);
        else setFlywheelVel(toaster_rpm);
      }

    }

  }

}
void ShooterShootParams::handleStateChange(SHOOTER_STATE_TYPES_VARIANT prev_state){
  // angler_p.setState(LOW);
  log("INIT shots_left %d\n", shots_left);
  intakeIndex();  // Sets intake to index state
  FLYWHEEL_STATE_TYPES_VARIANT temp_flywheel_state = flywheel.getState();
  // if(get_if<FlywheelMoveVelParams>(&temp_flywheel_state)->target_vel > 2000) get_if<FlywheelMoveVelParams>(&temp_flywheel_state)->kP = 0.7;
  // else get_if<FlywheelMoveVelParams>(&temp_flywheel_state)->kP = 0.5;

}

void shoot(int shots){
  shooter.changeState(ShooterShootParams{shots});
}