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
  // Odometry related variables
  double l_vel, r_vel, b_vel; // Velocities of each of the tracking wheel in inches/sec
  Mutex pos_mutex; // locks g_pos
  Position g_pos{};
  Position g_vel;

  // Movement related fields
  Position power; // power to apply to the drive motors
  std::atomic<double> drive_error; // How far the robot is from it's target 
  void waitForComplete(); // Waits until the motion completes
  void waitForDistance(double distance); // Waits until the robot is within a certain distance from it's target
  void reset(Position pos = {0.0, 0.0, 0.0}); // Resets the global tracking position to pos
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

void flattenAgainstWallSync();
void flattenAgainstWallAsync();

// Takes a function that returns an angle in radians
void turnToAngleInternal(std::function<double()> getAngleFunc, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = 2.0);
void aimAtRed();
void aimAtBlue();

// State machine stuff

// Forward declarations
struct DriveIdleParams;
struct DriveOpControlParams;
struct DriveMttParams;
struct DriveTurnToAngleParams;
struct DriveTurnToTargetParams;
struct DriveFlattenParams;

#define DRIVE_STATE_TYPES DriveIdleParams, DriveOpControlParams, DriveMttParams, DriveTurnToAngleParams, DriveTurnToTargetParams, DriveFlattenParams

using driveVariant = std::variant<DRIVE_STATE_TYPES>;

struct DriveIdleParams{
  inline static const std::string name = "DriveIdle";
  void handle();
  void handleStateChange(driveVariant prev_state);
};

struct DriveOpControlParams{
  inline static const std::string name = "DriveOpControl";
  void handle();
  void handleStateChange(driveVariant prev_state);
};

struct DriveMttParams{
  Vector target;
  E_Brake_Modes brake_mode = E_Brake_Modes::brake;
  uint8_t max_power = 127;
  double end_error_x = 1.0;
  E_Robot_Sides robot_side = E_Robot_Sides::automatic;

  DriveMttParams(Vector target, E_Brake_Modes brake_mode = E_Brake_Modes::brake, uint8_t max_power = 127, double end_error_x = 1.0, E_Robot_Sides robot_side = E_Robot_Sides::automatic);

  inline static const std::string name = "DriveMoveToTarget";
  void handle();
  void handleStateChange(driveVariant prev_state);
};

struct DriveTurnToAngleParams{
  double angle;
  E_Brake_Modes brake_mode = E_Brake_Modes::brake;
  double end_error = 2.0;
  
  DriveTurnToAngleParams(double angle, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = 2.0);

  inline static const std::string name = "DriveTurnToAngle";
  void handle();
  void handleStateChange(driveVariant prev_state);
};

struct DriveTurnToTargetParams{
  Vector target;
  bool reverse = false;
  E_Brake_Modes brake_mode = E_Brake_Modes::brake;
  double end_error = 2.0;
  
  DriveTurnToTargetParams(Vector target, bool reverse = false, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = 2.0);

  inline static const std::string name = "DriveTurnToTarget";
  void handle();
  void handleStateChange(driveVariant prev_state);
};

struct DriveFlattenParams{
  inline static const std::string name = "DriveFlatten";
  void handle();
  void handleStateChange(driveVariant prev_state);
};

extern Machine<DRIVE_STATE_TYPES> drive;