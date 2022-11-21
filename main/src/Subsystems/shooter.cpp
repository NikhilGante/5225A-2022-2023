#include "shooter.hpp"


Machine<SHOOTER_STATE_TYPES> shooter("shooter", ShooterIdleParams{});

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
  flywheel_state = flywheel.getState();
  // Fires shot if flywheel rpm is within 20 of target and 300 ms has elapsed
  if(shoot_timer.getTime() > 300 && fabs(std::get_if<FlywheelMoveVelParams>(&flywheel_state)->error.load() < 20)){
    printf("%d STARTED SHOOTING\n", millis());
    // shooting = 1000;
    indexer_p.setState(HIGH);	
    delay(75); // wait for SHOOTER to extend
    // delay(175);
    printf(" %d FINISHED SHOT\n", millis());
    // shooting = 0;
    indexer_p.setState(LOW);
    printf("%d FINISHED Retraction\n", millis());
    shots_left--;
    g_mag_disc_count--;

    if(shots_left <= 0){  // If shooting is done
      // Sets subsystems back to their state before shooting
      intakeOn();
      shooter.changeState(ShooterIdleParams{});
    }
    // delay(100);// wait for SHOOTER to retract
    delay(175);// wait for SHOOTER to retract
    shoot_timer.reset();
  }

}
void ShooterShootParams::handleStateChange(SHOOTER_STATE_TYPES_VARIANT prev_state){
  intakeIndex();  // Sets intake to index state
}
