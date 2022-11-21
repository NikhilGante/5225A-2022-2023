#include "flywheel.hpp"


Machine<FLYWHEEL_STATE_TYPES> flywheel("flywheel", FlywheelIdleParams{});

// Flywheel idle state

const char* FlywheelIdleParams::getName(){
  return "FlywheelIdle";
}
void FlywheelIdleParams::handle(){}
void FlywheelIdleParams::handleStateChange(FLYWHEEL_STATE_TYPES_VARIANT prev_state){}

// Flywheel off state

const char* FlywheelOffParams::getName(){
  return "FlywheelOff";
}
void FlywheelOffParams::handle(){}
void FlywheelOffParams::handleStateChange(FLYWHEEL_STATE_TYPES_VARIANT prev_state){
  flywheel_m.move(0);
}

// Flywheel move velocity state

FlywheelMoveVelParams::FlywheelMoveVelParams(int target_vel): target_vel(target_vel){}

atomic<double> FlywheelMoveVelParams::error; // Target vel - actual vel (global static var)

const char* FlywheelMoveVelParams::getName(){
  return "FlywheelMoveVel";
}
void FlywheelMoveVelParams::handle(){
  rot_vel = 3*60*flywheel_rot_sensor.get_velocity()/360;	// Actual velocity of flywheel
  
  error = target_vel - rot_vel;
  output = kB * target_vel + kP * error;
  output = std::clamp(output, -1.0, 127.0);	// Decelerates at -1.0 at the most
  flywheel_m.move(output);
  printf("%d, %d, %d, %d, %.2lf, %.2lf, %.2lf, %.2lf\n", millis(), flywheel_ds.get_value(), target_vel, rot_vel, error.load(), output, target_vel * kB, kP*error);

}
void FlywheelMoveVelParams::handleStateChange(FLYWHEEL_STATE_TYPES_VARIANT prev_state){}

void setFlywheelVel(int32_t vel){
  flywheel.changeState(FlywheelMoveVelParams{vel});
}