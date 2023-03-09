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
  public:
    int target_vel; //Don't make this const, because it screws up the variant
    double kP = 0.5;

    inline static const std::string name = "FlywheelMoveVel";
    void handle();
    void handleStateChange(flywheelVariant prev_state);

    FlywheelMoveVelParams(int target_vel);

  private:
    static constexpr double kB = 0.03735294117;
    static double rot_vel; // Velocity detected by rotation sensor
    static double output; // Power that goes to the flywheel motor

    static constexpr double smooth_val = 0.65; // Tuned to smooth velocity values
    static Timer motor_vel_read; // Ensures motor's velocity is calculated every 40ms
    static Timer log_timer;
    static double smoothed_vel;  // Velocity with exponential filter applied to it
    static double last_pos;  // Motor's position from previous cycle
    static double last_vel; // Smoothed velocity (from last cycle)
    static double manual_vel;  // Pre-smoothed velocity
};

void setFlywheelVel(int32_t vel);