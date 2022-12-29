#include "shooter.hpp"
#include "flywheel.hpp"
#include "intake.hpp"

bool angleOverride = false;

Machine<SHOOTER_STATE_TYPES> shooter("shooter", ShooterIdleParams{});

void shooterHandleInput(){
  SHOOTER_STATE_TYPES_VARIANT cur_state = shooter.getState();
  if(get_if<ShooterIdleParams>(&cur_state)){
    if(master.get_digital_new_press(tripleShotBtn)) shoot(3);
    if(master.get_digital_new_press(singleShotBtn)) shoot(1);
  }

  if(master.get_digital_new_press(anglerToggleBtn)) {
    if (!angleOverride){
      if (angler_p.getState()==0) setFlywheelVel(1850);
      else setFlywheelVel(1400);
    }
    angler_p.toggleState();
  } 

  if (master.get_digital_new_press(angleOverrideBtn)) {angleOverride = true; setFlywheelVel(1400);}


  
  
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
  if(fabs(flywheel_error) > 150) cycle_check.reset();

  // printf("cycle_check:%lld\n", cycle_check.getTime());
  // cycle_check.getTime() >= 30
  // flywheel_error.load() < 20
  g_mag_disc_count = 0;
  if(shoot_timer.getTime() > 400 && cycle_check.getTime() >= 30){
    printf("%d STARTED SHOOTING\n", millis());
    shoot_timer.reset();
    indexer_p.setState(HIGH);	
    delay(150); // wait for SHOOTER to extend
    printf(" %d FINISHED SHOT\n", millis());
    indexer_p.setState(LOW);
    printf("%d FINISHED Retraction\n", millis());
    shots_left--;
    

    delay(100);// wait for SHOOTER to retract
    if(shots_left <= 0){  // If shooting is done
      delay(100); // waits for last disc to shoot
      // Sets subsystems back to their state before shooting
      intakeOn();
      shooter.changeState(ShooterIdleParams{});

      if (!angleOverride){
        if (angler_p.getState()==0) setFlywheelVel(1850);
        else setFlywheelVel(1400);
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