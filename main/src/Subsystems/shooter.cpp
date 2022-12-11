#include "shooter.hpp"


Machine<SHOOTER_STATE_TYPES> shooter("shooter", ShooterIdleParams{});

void shooterHandleInput(){
  SHOOTER_STATE_TYPES_VARIANT cur_state = shooter.getState();
  if(get_if<ShooterIdleParams>(&cur_state)){
    if(master.get_digital_new_press(tripleShotBtn)) shoot(3);
    if(master.get_digital_new_press(singleShotBtn)) shoot(1);
  }

  if(master.get_digital_new_press(anglerToggleBtn)) angler_p.toggleState();
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
  if(shoot_timer.getTime() > 300 && fabs(flywheel_error) < 20){
    printf("%d STARTED SHOOTING\n", millis());
    shoot_timer.reset();
    indexer_p.setState(HIGH);	
    delay(75); // wait for SHOOTER to extend
    printf(" %d FINISHED SHOT\n", millis());
    indexer_p.setState(LOW);
    printf("%d FINISHED Retraction\n", millis());
    shots_left--;
    g_mag_disc_count--;

    delay(100);// wait for SHOOTER to retract
    if(shots_left <= 0){  // If shooting is done
      delay(100); // waits for last disc to shoot
      // Sets subsystems back to their state before shooting
      intakeOn();
      shooter.changeState(ShooterIdleParams{});
    }
    
  }

}
void ShooterShootParams::handleStateChange(SHOOTER_STATE_TYPES_VARIANT prev_state){
  angler_p.setState(LOW);
  intakeIndex();  // Sets intake to index state
}

void shoot(int shots){
  shooter.changeState(ShooterShootParams{shots});
}