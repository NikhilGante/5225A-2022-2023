#pragma once
#include "main.h"
#include "../config.hpp"
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

struct FlywheelMoveVelParams{
  int target_vel;

  static atomic<double> error; // Target vel - actual vel (global var)

  FlywheelMoveVelParams(int target_vel);

  const char* getName();
  void handle();
  void handleStateChange(FLYWHEEL_STATE_TYPES_VARIANT prev_state);
private:
  static constexpr double kB = 0.0385;	// Target velocity multiplied by this outputs a motor voltage
	static constexpr double kP = 0.5;
  int32_t rot_vel; // Velocity detected by rotation sensor
  double output; // Power that goes to the flywheel motor
};

void setFlywheelVel(int32_t vel); // Wrapper function to set flywheel to FlywheelMoveVel state