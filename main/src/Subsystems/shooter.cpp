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
    if(master.getNewDigital(tripleShotBtn)) shoot(3);
    if(master.getNewDigital(singleShotBtn)) shoot(1);
  }

  if(master.getNewDigital(anglerToggleBtn)) {
    angler_p.toggleState();
    if (!angleOverride){
      if (angler_p.getState() == 0) setFlywheelVel(barrier_rpm);
      else{
        if(goal_disturb)  setFlywheelVel(3600);
        else  setFlywheelVel(toaster_rpm);
      }
    }
  } 

  if (master.getNewDigital(angleOverrideBtn)) {
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

  if(master.getNewDigital(goalDisturbBtn)){
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
    if(std::abs(flywheel_error) > 150) cycle_check.reset();
  }

  // shoot_log("cycle_check:%lld", cycle_check.getTime());
  // cycle_check.getTime() >= 30
  // flywheel_error.load() < 20
  if(shoot_timer.getTime() > 400 && cycle_check.getTime() >= 30){
    shoot_log("%d STARTED SHOOTING", millis());
    shoot_timer.reset();
    indexer_p.setState(HIGH);	
    _Task::delay(150); // Waits for SHOOTER to extend
    shoot_log("%d FINISHED SHOT", millis());
    indexer_p.setState(LOW);
    shoot_log("%d FINISHED Retraction", millis());
    shots_left--;
    shoot_log("shots left: %d", shots_left);
    if (g_mag_disc_count > 0) g_mag_disc_count--;
    

    shoot_log("condition %d", shots_left <= 0);
    _Task::delay(100);// wait for SHOOTER to retract // DON'T CHANGE THIS LINE 
    shoot_log("condition2 %d", shots_left <= 0);

    if(shots_left <= 0){  // If shooting is done
      shoot_log("ENTERED SHOTS LEFT");
      g_mag_disc_count = 0;
      _Task::delay(100); // waits for last disc to shoot
      // Sets subsystems back to their state before shooting
      intakeOn();
      shooter.changeState(ShooterIdleParams{});
      _Task::delay(50);

      if (!angleOverride){
        if (angler_p.getState()==0) setFlywheelVel(barrier_rpm);
        else setFlywheelVel(toaster_rpm);
      }

    }
    shoot_log("if statement finished");
    _Task::delay(10);

    
  }

}
void ShooterShootParams::handleStateChange(shooterVariant prev_state){
  // angler_p.setState(LOW);
  shoot_log("INIT shots_left %d", shots_left);
  intakeIndex();  // Sets intake to index state
}

void shoot(int shots){
  shooter.changeState(ShooterShootParams{shots});
}