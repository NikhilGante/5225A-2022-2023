#include "flywheel.hpp"
#include "shooter.hpp"
#include "../config.hpp"
#include "../Devices/controller.hpp"
#include "../Devices/motor.hpp"
#include "../util.hpp"

constexpr double MS_TO_MIN = 60000;
constexpr double SPROCKET_RATIO = 1.0/1;
constexpr double CARTRIDGE_TO_RAW = 6;

// 11/27
// 28/60
// 43/88
// RPM is 2440 at far
// RPM is 2320 from midline
// 1700 from barrier
// RPM is 1400 for toaster shot
// 56 degrees up close
Machine<FLYWHEEL_STATE_TYPES> flywheel("flywheel", FlywheelMoveVelParams{barrier_rpm});

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
// int32_t FlywheelMoveVelParams::rot_vel; // Power that goes to the flywheel motor
double FlywheelMoveVelParams::output; // Power that goes to the flywheel motor
double FlywheelMoveVelParams::smoothed_vel;  // Velocity with exponential filter applied to it
double FlywheelMoveVelParams::last_pos;  // Motor's position from previous cycle
double FlywheelMoveVelParams::last_vel; // Smoothed velocity (from last cycle)
double FlywheelMoveVelParams::manual_vel;  // Pre-smoothed velocity

FlywheelMoveVelParams::FlywheelMoveVelParams(int target_vel): target_vel(target_vel){}

void FlywheelMoveVelParams::handle(){
  // rot_vel = -60*flywheel_rot_sensor.get_velocity()/360;	// Actual velocity of flywheel
  
  // error = target_vel - rot_vel;
  // output = kB * target_vel + kP * error;
  // output = std::clamp(output, -1.0, 127.0);	// Decelerates at -1.0 at the most
  // flywheel_m.move(output);
  // printf("%d, %d, %d, %d, %.2lf, %.2lf, %.2lf, %.2lf\n", millis(), flywheel_ds.get_value(), target_vel, rot_vel, error.load(), output, target_vel * kB, kP*error);

  // ***********************

  // if(master.getNewDigital(DIGITAL_UP))  target_vel += 25;
  // if(master.getNewDigital(DIGITAL_DOWN))  target_vel -= 25;

  // Calculating filtered velocity
  if(motor_vel_read.getTime() >= 40){
    const double pos = flywheel_m.getPosition() * CARTRIDGE_TO_RAW * MS_TO_MIN * SPROCKET_RATIO * deg_to_rot;
    manual_vel = (pos - last_pos)/motor_vel_read.getTime();
    last_pos = pos;

    smoothed_vel = manual_vel*(1-smooth_val) + last_vel*smooth_val;
    last_vel = smoothed_vel;

    motor_vel_read.reset();
  }

  // Velocity control
  flywheel_error = target_vel - smoothed_vel;
  // double correction = sgn(flywheel_error.load())*pow(0.07*flywheel_error, 2);
  const double correction = flywheel_error*kP;
  // if(std::abs(correction) > 2500) correction = 2500;
  output = kB * target_vel + correction;
  output = std::clamp(output, -1.0, 127.0);	// decelerates at -1.0 at the most
  // output = 127;
  
  // printf("%d, %d, %d, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %lf\n", millis(), shooter_ds.get_value(), target_vel, flywheel_error.load(), output, target_vel * kB, correction, smoothed_vel, intake_m.get_actual_velocity());

  if (flywheel_m.getTemperature() >= 50){
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