#pragma once
#include "main.h"
#include "../config.hpp"
#include "../Libraries/timer.hpp"
#include "../Libraries/state.hpp"
#include <atomic>

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

extern std::atomic<double> flywheel_error; // Target vel - actual vel (global static var)

struct FlywheelMoveVelParams{
  int target_vel;

  FlywheelMoveVelParams(int target_vel);

  const char* getName();
  void handle();
  void handleStateChange(FLYWHEEL_STATE_TYPES_VARIANT prev_state);
private:
  static constexpr double kB = 0.0508;	// Target velocity multiplied by this outputs a motor voltage
	static constexpr double kP = 0.95;
  static int32_t rot_vel; // Velocity detected by rotation sensor
  static double output; // Power that goes to the flywheel motor

  static constexpr double smooth_val = 0.65; // Tuned to smooth velocity values
  Timer motor_vel_read{"motor_vel_read"}; // Ensures motor's velocity is calculated every 40ms
  static double smoothed_vel;  // Velocity with exponential filter applied to it
  static double last_pos;  // Motor's position from previous cycle
  static double last_vel; // Smoothed velocity (from last cycle)
  static double manual_vel;  // Pre-smoothed velocity
};
