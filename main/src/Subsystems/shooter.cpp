#include "shooter.hpp"
#include "flywheel.hpp"
#include "intake.hpp"

const int toaster_rpm = 1400;
const int barrier_rpm = 1820;
bool goal_disturb = false;

// 1820
bool angleOverride = false;
bool flywheelOff = false;


Machine<SHOOTER_STATE_TYPES> shooter("shooter", ShooterIdleParams{});

void shooterHandleInput(){
  SHOOTER_STATE_TYPES_VARIANT cur_state = shooter.getState();
  if(get_if<ShooterIdleParams>(&cur_state)){
    if(master.get_digital_new_press(tripleShotBtn)) shoot(3);
    if(master.get_digital_new_press(singleShotBtn)) shoot(1);
    // if(master.get_digital_new_press(flywheelOffBtn)) flywheelOff = !flywheelOff;
  }

  if(master.get_digital_new_press(anglerToggleBtn) && !flywheelOff) {
    angler_p.toggleState();
    if (!angleOverride){
      if (angler_p.getState()==0) setFlywheelVel(barrier_rpm);
      else{
        if(goal_disturb)  setFlywheelVel(3600);
        else  setFlywheelVel(toaster_rpm);
      }
    }
  } 

  if (master.get_digital_new_press(angleOverrideBtn) && !flywheelOff) {
    angleOverride = !angleOverride; 
    if(angleOverride) setFlywheelVel(toaster_rpm);
    else{
      if (angler_p.getState()==0) setFlywheelVel(barrier_rpm);
      else{
        if(goal_disturb)  setFlywheelVel(3600);
        else  setFlywheelVel(toaster_rpm);
      }
    }
  }

  if(master.get_digital_new_press(goalDisturbBtn))  goal_disturb = !goal_disturb;

  
  
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
  // Fires shot if flywheel rpm is within 20 of target and 300 ms has elapsed
  if(goal_disturb){
    if(fabs(flywheel_error) > 1000) cycle_check.reset();
  }
  else{
    if(fabs(flywheel_error) > 150) cycle_check.reset();
  }

  // printf("cycle_check:%lld\n", cycle_check.getTime());
  // cycle_check.getTime() >= 30
  // flywheel_error.load() < 20
  if(shoot_timer.getTime() > 400 && cycle_check.getTime() >= 30){
    printf("%d STARTED SHOOTING\n", millis());
    shoot_timer.reset();
    indexer_p.setState(HIGH);	
    delay(150); // Waits for SHOOTER to extend
    printf(" %d FINISHED SHOT\n", millis());
    indexer_p.setState(LOW);
    printf("%d FINISHED Retraction\n", millis());
    shots_left--;
    if (g_mag_disc_count > 0) g_mag_disc_count--;
    

    _Task::delay(100);// wait for SHOOTER to retract
    if(shots_left <= 0){  // If shooting is done
      _Task::delay(100); // waits for last disc to shoot
      // Sets subsystems back to their state before shooting
      intakeOn();
      shooter.changeState(ShooterIdleParams{});
      _Task::delay(50);

      if (!angleOverride && !flywheelOff){
        if (angler_p.getState()==0) setFlywheelVel(barrier_rpm);
        else setFlywheelVel(toaster_rpm);
      }

    }
    
  }

}
void ShooterShootParams::handleStateChange(SHOOTER_STATE_TYPES_VARIANT prev_state){
  // angler_p.setState(LOW);
  intakeIndex();  // Sets intake to index state
}

void shoot(int shots){
  shooter.changeState(ShooterShootParams{shots});
}