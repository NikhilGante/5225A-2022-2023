#pragma once
#include "main.h"
#include "Libraries/logging.hpp"
#include "Libraries/geometry.hpp"
#include "drive.hpp"
#include "Libraries/pid.hpp"
#include "Libraries/state.hpp"

enum class E_Brake_Modes{
  none, // the robot will keep going at whatever speed it was already going at
  coast,  // drivebase motors will turn off
  brake // the drivebase motors will hold
};

enum class E_Robot_Sides{
  front,
  back,
  automatic // automatically deduce which side
};

class Tracking{
  
public:
  const double min_move_power_y = 30.0, min_move_power_a = 30.0;
  // odometry related variables
  double l_vel, r_vel, b_vel; // velocities of each of the tracking wheel in inches/sec
  Position g_pos{};
  Position g_vel;

  // movement related fields
  Position power; // power to apply to the drive motors
  atomic<double> drive_error; // how far the robot is from it's target 
  void waitForComplete(); // waits until the motion completes
  void waitForDistance(double distance); // waits until the robot is within a certain distance from it's target

  // // moveToTarget is declared a friend so it can access private tracking properties
  // friend void moveToTarget(Position, E_Brake_Modes, double max_power, double exit_power, bool overshoot, double end_error_d, double end_error_a);

};
extern Tracking tracking;

extern Vector r_goal, b_goal;

void trackingUpdate();
void handleBrake(E_Brake_Modes brake_mode); // Brakes depending on type of brake mode passed in

// Wrapper functions for drive states (motion algorithms)
void moveToTargetSync(Vector target, E_Brake_Modes brake_mode = E_Brake_Modes::brake, uint8_t max_power = 127, double end_error_x = 1.0, E_Robot_Sides robot_side = E_Robot_Sides::automatic);
void moveToTargetAsync(Vector target, E_Brake_Modes brake_mode = E_Brake_Modes::brake, uint8_t max_power = 127, double end_error_x = 1.0, E_Robot_Sides robot_side = E_Robot_Sides::automatic);

void turnToAngleSync(double angle, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = 2.0);
void turnToAngleAsync(double angle, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = 2.0);

void turnToTargetSync(Vector target, bool reverse = false, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = 2.0);
void turnToTargetAsync(Vector target, bool reverse = false, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = 2.0);


// Takes a function that returns an angle in radians
void turnToAngleInternal(function<double()> getAngleFunc, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = 2.0);
void aimAtRed();
void aimAtBlue();

// state machine stuff

// Forward declarations
struct DriveIdleParams;
struct DriveMttParams;
struct DriveTurnToAngleParams;
struct DriveTurnToTargetParams;
struct DriveResetParams;

#define DRIVE_STATE_TYPES DriveIdleParams, DriveMttParams, DriveTurnToAngleParams, DriveTurnToTargetParams, DriveResetParams

#define DRIVE_STATE_TYPES_VARIANT std::variant<DRIVE_STATE_TYPES>

struct DriveIdleParams{
  const char* getName();
  void handle();
  void handleStateChange(DRIVE_STATE_TYPES_VARIANT prev_state);
};

struct DriveMttParams{
  Vector target;
  E_Brake_Modes brake_mode = E_Brake_Modes::brake;
  uint8_t max_power = 127;
  double end_error_x = 1.0;
  E_Robot_Sides robot_side = E_Robot_Sides::automatic;

  DriveMttParams(Vector target, E_Brake_Modes brake_mode = E_Brake_Modes::brake, uint8_t max_power = 127, double end_error_x = 1.0, E_Robot_Sides robot_side = E_Robot_Sides::automatic);

  const char* getName();
  void handle();
  void handleStateChange(DRIVE_STATE_TYPES_VARIANT prev_state);
};

struct DriveTurnToAngleParams{
  double angle;
  E_Brake_Modes brake_mode = E_Brake_Modes::brake;
  double end_error = 2.0;
  
  DriveTurnToAngleParams(double angle, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = 2.0);

  const char* getName();
  void handle();
  void handleStateChange(DRIVE_STATE_TYPES_VARIANT prev_state);
};

struct DriveTurnToTargetParams{
  Vector target;
  bool reverse = false;
  E_Brake_Modes brake_mode = E_Brake_Modes::brake;
  double end_error = 2.0;
  
  DriveTurnToTargetParams(Vector target, bool reverse = false, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = 2.0);

  const char* getName();
  void handle();
  void handleStateChange(DRIVE_STATE_TYPES_VARIANT prev_state);
};

struct DriveResetParams{
  const char* getName();
  void handle();
  void handleStateChange(DRIVE_STATE_TYPES_VARIANT prev_state);
};

extern Machine<DRIVE_STATE_TYPES> drive;