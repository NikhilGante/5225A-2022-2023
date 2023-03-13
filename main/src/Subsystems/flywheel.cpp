#include "flywheel.hpp"
#include "pros/misc.hpp"
#include "shooter.hpp"
#include "../config.hpp"
#include "../Devices/controller.hpp"
#include "../Devices/motor.hpp"
#include "../Devices/piston.hpp"
#include "../Devices/others.hpp"
#include "../util.hpp"
#include "../Libraries/logging.hpp"

constexpr double MS_TO_MIN = 60000;
constexpr double SPROCKET_RATIO = 1.0/1;
constexpr double CARTRIDGE_TO_RAW = 6;

Timer FlywheelMoveVelParams::motor_vel_read{"Motor Velocity Read", flywheel_log}; // Ensures motor's velocity is calculated every 40ms
Timer FlywheelMoveVelParams::log_timer{"Log Timer", flywheel_log};
Timer FlywheelMoveVelParams::disc_correction_timer{"Disc Correction", flywheel_log, false}; // Doesn't apply correction while disc is passing through

// 11/27
// 28/60
// 43/88
// RPM is 2440 at far
// RPM is 2320 from midline
// 1700 from barrier
// RPM is 1400 for toaster shot
// 56 degrees up close
Machine<FLYWHEEL_STATE_TYPES> flywheel{"Flywheel", FlywheelOffParams{}};

// Flywheel idle state

void FlywheelIdleParams::handle(){}
void FlywheelIdleParams::handleStateChange(flywheelVariant prev_state){}

// Flywheel off state

void FlywheelOffParams::handle(){}
void FlywheelOffParams::handleStateChange(flywheelVariant prev_state) {flywheel_m.move(0);}

// Flywheel move velocity state

std::atomic<double> flywheel_error; // Target vel - actual vel (global static var)

// static vars
double FlywheelMoveVelParams::rot_vel; // Power that goes to the flywheel motor
double FlywheelMoveVelParams::output; // Power that goes to the flywheel motor
double FlywheelMoveVelParams::smoothed_vel;  // Velocity with exponential filter applied to it
double FlywheelMoveVelParams::last_pos;  // Motor's position from previous cycle
double FlywheelMoveVelParams::last_vel; // Smoothed velocity (from last cycle)
double FlywheelMoveVelParams::manual_vel;  // Pre-smoothed velocity

FlywheelMoveVelParams::FlywheelMoveVelParams(int target_vel): target_vel(target_vel){}

void FlywheelMoveVelParams::handle(){
  rot_vel = flywheel_rot_sensor.getVel()/60.0;	// Actual velocity of flywheel

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
  flywheel_error = target_vel - rot_vel;

  // Flywheel doesn't correct vel while disc is passing through
  const double correction = competition::is_autonomous() && disc_correction_timer.getTime() < 250 && disc_correction_timer.playing() ? 0 : flywheel_error*kP;

  output = kB * target_vel + correction;
  output = std::clamp(output, -5.0, 127.0);
  // output = 127;
  
  // flywheel_log("%d, %d, %d, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %d\n", millis(), shooter_ds.get_value()+1000, target_vel, flywheel_error.load(), output, target_vel * kB, correction, rot_vel, smoothed_vel, mag_ds.get_value());

  if(log_timer.getTime() > 10 || shooter_ds.getVal() < 600){
    if (shooter_ds.getVal() < 600){
      disc_correction_timer.reset();  // Flywheel rpm shouldn't be corrected for the next 200 ms
      flywheel_log("DISC CONTACTED FLYWHEEL\n");

    }
    // if (shooter_ds.getVal() < 800) flywheel_log("DISC CONTACTED FLYWHEEL , %d, %d, %d, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %lf, %d\n", millis(), shooter_ds.get_value()+1000, target_vel, flywheel_error.load(), output, target_vel * kB, correction, rot_vel, intake_m.get_actual_velocity(), angler_p.getState());
    // flywheel_log("FLYWHEEL , %d, %d, %d, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %lf\n", millis(), shooter_ds.get_value()+1000, target_vel, flywheel_error.load(), output, target_vel * kB, correction, rot_vel, intake_m.get_actual_velocity());
    log_timer.reset();
  }

  if(!master.connected()){
    WAIT_UNTIL(master.connected());
    flywheel_m.move(0);
    delay(100);
  }

  static bool flywheelOn = true;
  if(master.getNewDigital(goalDisturbBtn)) flywheelOn = !flywheelOn;

  flywheel_m.move(flywheelOn ? output : 0);
  
  _Task::delay(10);
}

void FlywheelMoveVelParams::handleStateChange(flywheelVariant prev_state) {log_timer.reset();}

void setFlywheelVel(int32_t vel){
  flywheel_log("Flywheel was changed to velocity %d", vel);
  flywheel.changeState(FlywheelMoveVelParams{vel});
}