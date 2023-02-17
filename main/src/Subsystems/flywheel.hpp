#pragma once
#include "../Libraries/state.hpp"
#include "../Libraries/timer.hpp"

// forward declarations
struct FlywheelIdleParams;
struct FlywheelOffParams;
struct FlywheelMoveVelParams;

#define FLYWHEEL_STATE_TYPES FlywheelIdleParams, FlywheelOffParams, FlywheelMoveVelParams

using flywheelVariant = std::variant<FLYWHEEL_STATE_TYPES>;

extern Machine<FLYWHEEL_STATE_TYPES> flywheel;

struct FlywheelIdleParams{
  inline static const std::string name = "FlywheelIdle";
  void handle();
  void handleStateChange(flywheelVariant prev_state);
};

struct FlywheelOffParams{  
  inline static const std::string name = "FlywheelOff";
  void handle();
  void handleStateChange(flywheelVariant prev_state);
};

extern std::atomic<double> flywheel_error; // Target vel - actual vel (global static var)

struct FlywheelMoveVelParams{
  int target_vel;
  Timer flywheel_print{"flywheel_print"};
	double kP = 0.7;

  FlywheelMoveVelParams(int target_vel);

  inline static const std::string name = "FlywheelMoveVel";
  void handle();
  void handleStateChange(flywheelVariant prev_state);
private:
  static constexpr double kB = 0.0332;	// Target velocity multiplied by this outputs a motor voltage
  static double rot_vel; // Velocity detected by rotation sensor
  static double output; // Power that goes to the flywheel motor

  static constexpr double smooth_val = 0.65; // Tuned to smooth velocity values
  Timer motor_vel_read{"motor_vel_read"}; // Ensures motor's velocity is calculated every 40ms
  static double smoothed_vel;  // Velocity with exponential filter applied to it
  static double last_pos;  // Motor's position from previous cycle
  static double last_vel; // Smoothed velocity (from last cycle)
  static double manual_vel;  // Pre-smoothed velocity
};

void setFlywheelVel(int32_t vel, int line = -1);