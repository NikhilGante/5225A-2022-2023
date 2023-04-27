#include "flywheel.hpp"
#include "pros/misc.hpp"
#include "shooter.hpp"
#include "../Libraries/logging.hpp"
#include "../drive.hpp"

#define DEG_TO_ROT 1/360
#define MS_TO_MIN 60000
#define SPROCKET_RATIO 1/1
#define CARTRIDGE_TO_RAW 6

// 11/27
// 28/60
// 43/88
// RPM is 2440 at far
// RPM is 2320 from midline
// 1700 from barrier
// RPM is 1400 for toaster shot
// 56 degrees up close
Machine<FLYWHEEL_STATE_TYPES> flywheel("flywheel", FlywheelOffParams{});

bool flywheelOn = true;


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

atomic<double> flywheel_error; // Target vel - actual vel (global static var)

// static vars
double FlywheelMoveVelParams::rot_vel; // Power that goes to the flywheel motor
double FlywheelMoveVelParams::output; // Power that goes to the flywheel motor
double FlywheelMoveVelParams::smoothed_vel;  // Velocity with exponential filter applied to it
double FlywheelMoveVelParams::last_pos;  // Motor's position from previous cycle
double FlywheelMoveVelParams::last_vel; // Smoothed velocity (from last cycle)
double FlywheelMoveVelParams::manual_vel;  // Pre-smoothed velocity

FlywheelMoveVelParams::FlywheelMoveVelParams(int target_vel): target_vel(target_vel){}

const char* FlywheelMoveVelParams::getName(){
  return "FlywheelMoveVel";
}
void FlywheelMoveVelParams::handle(){
  rot_vel = 60*(double)flywheel_rot_sensor.get_velocity()/360;	// Actual velocity of flywheel

  // Calculating filtered velocity
  /*
  if(motor_vel_read.getTime() >= 40){
    double pos = flywheel_m.get_position() * CARTRIDGE_TO_RAW * MS_TO_MIN * SPROCKET_RATIO * DEG_TO_ROT;
    manual_vel = (pos - last_pos)/motor_vel_read.getTime();
    last_pos = pos;

    smoothed_vel = manual_vel*(1-smooth_val) + last_vel*smooth_val;
    last_vel = smoothed_vel;

    motor_vel_read.reset();
  }
  */

  // Velocity control
  flywheel_error = target_vel - rot_vel;

  double correction = flywheel_error*kP;

  output = kB * target_vel + correction;
  output = std::clamp(output, -5.0, 127.0);
  // output = 127;
  
  // log("%d, %d, %d, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %d\n", millis(), shooter_ds.get_value()+1000, target_vel, flywheel_error.load(), output, target_vel * kB, correction, rot_vel, smoothed_vel, mag_ds.get_value());

  if(log_timer.getTime() > 120 && shooter_ds.get_value() < SHOOTER_DS_THRESH){
    disc_correction_timer.reset();  // Flywheel rpm shouldn't be corrected for the next 200 ms
    log("DISC CONTACTED FLYWHEEL | %d, %d, %d, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf\n", millis(), shooter_ds.get_value()+1000, target_vel, flywheel_error.load(), output, target_vel * kB, correction, rot_vel);
    // log("DISC CONTACTED FLYWHEEL\n");
    log_timer.reset();

    // if (shooter_ds.get_value() < 800) log("DISC CONTACTED FLYWHEEL , %d, %d, %d, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %lf, %d\n", millis(), shooter_ds.get_value()+1000, target_vel, flywheel_error.load(), output, target_vel * kB, correction, rot_vel, intake_m.get_actual_velocity(), angler_p.getState());
  }
  

  if(!master.is_connected()){
    log("%lld CONNECTION LOST, RESTARTING FLYWHEEL\n", op_control_timer.getTime());
    WAIT_UNTIL(master.is_connected());
    flywheel_m.move(0);
    delay(100);
    
  }

  if(master.get_digital_new_press(flywheelToggleBtn)) flywheelOn = true;

  if(flywheelOn) flywheel_m.move(output);
  else flywheel_m.move(0);
  
}


void FlywheelMoveVelParams::handleStateChange(FLYWHEEL_STATE_TYPES_VARIANT prev_state){
  log_timer.reset();
}

void setFlywheelVel(int32_t vel, int line){
  log("Flywheel was changed on %d to velocity %d\n", line, vel);
  flywheel.changeState(FlywheelMoveVelParams{vel});
}