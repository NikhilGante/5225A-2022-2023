#include "flywheel.hpp"
#include "../config.hpp"
#include "../Libraries/controller.hpp"
#include "../Libraries/motor.hpp"

constexpr double DEG_TO_ROT = 1/360.0;
constexpr double MS_TO_MIN = 60000.0;
constexpr double SPROCKET_RATIO = 1/1.0;
constexpr double CARTRIDGE_TO_RAW = 6.0;

Machine<FLYWHEEL_STATE_TYPES> flywheel("flywheel", FlywheelMoveVelParams{2250});

// Flywheel idle state

void FlywheelIdleParams::handle(){}
void FlywheelIdleParams::handleStateChange(flywheelVariant prev_state){}

// Flywheel off state

void FlywheelOffParams::handle(){}
void FlywheelOffParams::handleStateChange(flywheelVariant prev_state){
  flywheel_m.move(0);
}

// Flywheel move velocity state

std::atomic<double> flywheel_error; // Target vel - actual vel (global static var)

// static vars
double FlywheelMoveVelParams::output; // Power that goes to the flywheel motor
double FlywheelMoveVelParams::smoothed_vel;  // Velocity with exponential filter applied to it
double FlywheelMoveVelParams::last_pos;  // Motor's position from previous cycle
double FlywheelMoveVelParams::last_vel; // Smoothed velocity (from last cycle)
double FlywheelMoveVelParams::manual_vel;  // Pre-smoothed velocity

FlywheelMoveVelParams::FlywheelMoveVelParams(int target_vel): target_vel(target_vel){}

void FlywheelMoveVelParams::handle(){
  // rot_vel = 3*60*flywheel_rot_sensor.get_velocity()/360;	// Actual velocity of flywheel
  
  // error = target_vel - rot_vel;
  // output = kB * target_vel + kP * error;
  // output = std::clamp(output, -1.0, 127.0);	// Decelerates at -1.0 at the most
  // flywheel_m.move(output);
  // printf("%d, %d, %d, %d, %.2lf, %.2lf, %.2lf, %.2lf\n", millis(), flywheel_ds.get_value(), target_vel, rot_vel, error.load(), output, target_vel * kB, kP*error);

// ***********************

  // Calculating filtered velocity
  if(motor_vel_read.getTime() >= 40){
    double pos = flywheel_m.getPosition() * CARTRIDGE_TO_RAW * MS_TO_MIN * SPROCKET_RATIO * DEG_TO_ROT;
    manual_vel = (pos - last_pos)/motor_vel_read.getTime();
    last_pos = pos;

    smoothed_vel = manual_vel*(1-smooth_val) + last_vel*smooth_val;
    last_vel = smoothed_vel;

    motor_vel_read.reset();
  }

  // Velocity control
  flywheel_error = target_vel - smoothed_vel;
  // double correction = sgn(flywheel_error.load())*pow(0.07*flywheel_error, 2);
  double correction = flywheel_error*kP;
  if(std::abs(correction) > 2500) correction = 2500;
  output = kB * target_vel + correction;
  output = std::clamp(output, -1.0, 127.0);	// decelerates at -1.0 at the most

  printf("%d, %d, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf \n", millis(), target_vel, flywheel_error.load(), output, target_vel * kB, correction, smoothed_vel);


  if (flywheel_m.getTemperature() >= 45){
    master.rumble("-");
    flywheel_m.move(0);
    WAIT_UNTIL(false);

  }
  flywheel_m.move(output);
  // flywheel_m.move(60);
}
void FlywheelMoveVelParams::handleStateChange(flywheelVariant prev_state){}

void setFlywheelVel(int32_t vel){
  flywheel.changeState(FlywheelMoveVelParams{vel});
}