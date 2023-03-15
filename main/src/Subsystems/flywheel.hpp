#pragma once
#include "main.h"
#include "../config.hpp"
#include "../Libraries/state.hpp"
#include <atomic>

#define SHOOTER_DS_THRESH 1000

// forward declarations
struct FlywheelIdleParams;
struct FlywheelOffParams;
struct FlywheelMoveVelParams;

#define FLYWHEEL_STATE_TYPES FlywheelIdleParams, FlywheelOffParams, FlywheelMoveVelParams

#define FLYWHEEL_STATE_TYPES_VARIANT std::variant<FLYWHEEL_STATE_TYPES>

extern Machine<FLYWHEEL_STATE_TYPES> flywheel;

struct FlywheelIdleParams{
  const char* getName();
  void handle();
  void handleStateChange(FLYWHEEL_STATE_TYPES_VARIANT prev_state);
};

struct FlywheelOffParams{  
  const char* getName();
  void handle();
  void handleStateChange(FLYWHEEL_STATE_TYPES_VARIANT prev_state);
};

extern atomic<double> flywheel_error; // Target vel - actual vel (global static var)

struct FlywheelMoveVelParams{
  int target_vel;
  Timer flywheel_print{"flywheel_print"};
	double kP = 0.5;

  FlywheelMoveVelParams(int target_vel);

  const char* getName();
  void handle();
  void handleStateChange(FLYWHEEL_STATE_TYPES_VARIANT prev_state);
private:

  // static constexpr double kB = 0.0332;	// Target velocity multiplied by this outputs a motor voltage
  static constexpr double kB = 0.03735294117;
  static double rot_vel; // Velocity detected by rotation sensor
  static double output; // Power that goes to the flywheel motor

  static constexpr double smooth_val = 0.65; // Tuned to smooth velocity values
  Timer motor_vel_read{"motor_vel_read"}; // Ensures motor's velocity is calculated every 40ms
  Timer log_timer{"log_timer"}; // Ensures motor's velocity is calculated every 40ms
  Timer disc_correction_timer{"disc_correction_timer", false}; // Doesn't apply correction while disc is passing through
  static double smoothed_vel;  // Velocity with exponential filter applied to it
  static double last_pos;  // Motor's position from previous cycle
  static double last_vel; // Smoothed velocity (from last cycle)
  static double manual_vel;  // Pre-smoothed velocity
};

void setFlywheelVel(int32_t vel, int line = -1);