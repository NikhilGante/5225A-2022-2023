#include "shooter.hpp"
#include "intake.hpp"
#include "../config.hpp"
#include "../Libraries/controller.hpp"
#include "../Libraries/piston.hpp"

static constexpr int toaster_rpm = 1400;

bool goal_disturb = false;
bool angleOverride = false;

Machine<SHOOTER_STATE_TYPES> shooter("shooter", ShooterIdleParams{});

void shooterHandleInput(){
  shooterVariant cur_state = shooter.getState();
  if(std::get_if<ShooterIdleParams>(&cur_state)){
    if(master.get_digital_new_press(tripleShotBtn)) shoot(3);
    if(master.get_digital_new_press(singleShotBtn)) shoot(1);
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
    if(angleOverride) setFlywheelVel(toaster_rpm);
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

void ShooterIdleParams::handle(){}
void ShooterIdleParams::handleStateChange(shooterVariant prev_state){}

// Shooter shoot state

ShooterShootParams::ShooterShootParams(int shots): shots_left(shots){}

void ShooterShootParams::handle(){
  // Fires shot if flywheel rpm is within 20 of target and 300 ms has elapsed
  if(goal_disturb){
    if(std::abs(flywheel_error) > 1000) cycle_check.reset();
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
  if(shoot_timer.getTime() > 400 && cycle_check.getTime() >= 30){
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

      if (!angleOverride){
        if (angler_p.getState()==0) setFlywheelVel(barrier_rpm);
        else setFlywheelVel(toaster_rpm);
      }

    }

  }

}
void ShooterShootParams::handleStateChange(shooterVariant prev_state){
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