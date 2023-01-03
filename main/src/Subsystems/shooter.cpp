#include "shooter.hpp"
#include "intake.hpp"
#include "../config.hpp"
#include "../Libraries/controller.hpp"
#include "../Libraries/piston.hpp"

const int toaster_rpm = 1400;
const int barrier_rpm = 1875;

bool angleOverride = false;
bool flywheelOff = false;

Machine<SHOOTER_STATE_TYPES> shooter("shooter", ShooterIdleParams{});

void shooterHandleInput(){
  shooterVariant cur_state = shooter.getState();
  if(std::get_if<ShooterIdleParams>(&cur_state)){
    if(master.get_digital_new_press(tripleShotBtn)) shoot(3);
    if(master.get_digital_new_press(singleShotBtn)) shoot(1);
    if(master.get_digital_new_press(flywheelOffBtn)) flywheelOff = !flywheelOff;
  }

  if(master.get_digital_new_press(anglerToggleBtn) && !flywheelOff) {
    angler_p.toggleState();
    if (!angleOverride){
      if (angler_p.getState()==0) setFlywheelVel(barrier_rpm);
      else setFlywheelVel(toaster_rpm);
    }
  } 

  if (master.get_digital_new_press(angleOverrideBtn) && !flywheelOff) {
    angleOverride = !angleOverride; 
    if(angleOverride) setFlywheelVel(toaster_rpm);
    else{
      if (angler_p.getState()==0) setFlywheelVel(barrier_rpm);
      else setFlywheelVel(toaster_rpm);
    }
  }


  
  
}

// Shooter idle state

void ShooterIdleParams::handle(){}
void ShooterIdleParams::handleStateChange(shooterVariant prev_state){}

// Shooter shoot state

ShooterShootParams::ShooterShootParams(int shots): shots_left(shots){}

void ShooterShootParams::handle(){
  // Fires shot if flywheel rpm is within 20 of target and 300 ms has elapsed
  if(std::abs(flywheel_error) > 150) cycle_check.reset();

  // printf("cycle_check:%lld\n", cycle_check.getTime());
  // cycle_check.getTime() >= 30
  // flywheel_error.load() < 20
  if(shoot_timer.getTime() > 400 && cycle_check.getTime() >= 30){
    printf("%d STARTED SHOOTING\n", millis());
    shoot_timer.reset();
    indexer_p.setState(HIGH);	
    delay(150); // wait for SHOOTER to extend
    printf(" %d FINISHED SHOT\n", millis());
    indexer_p.setState(LOW);
    printf("%d FINISHED Retraction\n", millis());
    shots_left--;
    if (g_mag_disc_count > 0) g_mag_disc_count--;
    

    delay(100);// wait for SHOOTER to retract
    if(shots_left <= 0){  // If shooting is done
      delay(100); // waits for last disc to shoot
      // Sets subsystems back to their state before shooting
      intakeOn();
      shooter.changeState(ShooterIdleParams{});
      delay(50);

      if (!angleOverride && !flywheelOff){
        if (angler_p.getState()==0) setFlywheelVel(barrier_rpm);
        else setFlywheelVel(toaster_rpm);
      }

    }
    
  }

}
void ShooterShootParams::handleStateChange(shooterVariant prev_state){
  // angler_p.setState(LOW);
  intakeIndex();  // Sets intake to index state
}

void shoot(int shots){
  shooter.changeState(ShooterShootParams{shots});
}