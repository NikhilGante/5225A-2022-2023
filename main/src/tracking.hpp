#pragma once
#include "main.h"
#include "Libraries/logging.hpp"
#include "Libraries/geometry.hpp"
#include "drive.hpp"
#include "Libraries/pid.hpp"
#include "Libraries/state.hpp"


constexpr double DRIVEBASE_WIDTH = 13.5;
constexpr double LEFT_DIST_OFFSET = 0.75;  // How far in the left sensor is from left edge
constexpr double RIGHT_DIST_OFFSET = 0.75;  // How far in the right sensor is from right edge
constexpr double BACK_DIST_OFFSET = 6.9375;  // How far in the ultrasonics are from tracking centre
constexpr double DISTANCE_DIST_OFFSET = 2.0;  // How far the distance sensor is from the tracking center on the local x axis


#define MAX_TURNING_POWER 127
#define TURNING_END_ERROR 1.5

#define MAX_DRIVE_POWER 127


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

enum class resetPosition {
   leftHome = 0,
   rightAway = 1,
   leftAway = 2,
   rightHome = 3
};



double getDistL();  // Gets tracking centre's position from wall on left
double getDistR();  // Gets tracking centre's position from wall on right
double getDistBack(); // Gets tracking centre's position from back wall
Position distanceReset(resetPosition pos);


class Tracking{
  
public:
  const double min_move_power_y = 25.0, min_move_power_a = 35.0;
  // Odometry related variables
  double l_vel, r_vel, b_vel; // Velocities of each of the tracking wheel in inches/sec
  Mutex pos_mutex; // locks g_pos
  Position g_pos{};
  Position g_vel;

  // Movement related fields
  Position power; // power to apply to the drive motors
  atomic<double> drive_error; // How far the robot is from it's target 
  void waitForComplete(); // Waits until the motion completes
  void waitForDistance(double distance); // Waits until the robot is within a certain distance from it's target
  void reset(Position pos = {0.0, 0.0, 0.0}); // Resets the global tracking position to pos
  void savePosToSD(); // Writes tracking pos to file on SD
  void loadPosFromSD(); // Loads position from file on SD into tracking pos
};
extern Tracking tracking;

extern Vector r_goal, b_goal;

void trackingUpdate();
void handleBrake(E_Brake_Modes brake_mode); // Brakes depending on type of brake mode passed in

// Wrapper functions for drive states (motion algorithms)
void moveToTargetSync(Vector target, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double max_power = MAX_DRIVE_POWER, double end_error_x = 1.0, double exit_power = 0.0, E_Robot_Sides robot_side = E_Robot_Sides::automatic);
void moveToTargetAsync(Vector target, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double max_power = MAX_DRIVE_POWER, double end_error_x = 1.0, double exit_power = 0.0, E_Robot_Sides robot_side = E_Robot_Sides::automatic);

void turnToAngleSync(double angle, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = TURNING_END_ERROR, double max_power = MAX_TURNING_POWER);
void turnToAngleAsync(double angle, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = TURNING_END_ERROR, double max_power = MAX_TURNING_POWER);

void turnToTargetSync(Vector target, double offset = 0.0, bool reverse = false, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = TURNING_END_ERROR, double max_power = MAX_TURNING_POWER);
void turnToTargetAsync(Vector target, double offset = 0.0, bool reverse = false, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = TURNING_END_ERROR, double max_power = MAX_TURNING_POWER);

void flattenAgainstWallSync();
void flattenAgainstWallAsync();

// Takes a function that returns an angle in radians
void turnToAngleInternal(function<double()> getAngleFunc, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = TURNING_END_ERROR, double max_power = MAX_TURNING_POWER);
void aimAtRed(double offset = 0.0,  double max_power = 70.0, double end_error = TURNING_END_ERROR);
void aimAtBlue(double offset = 0.0, double max_power = 70.0, double end_error = TURNING_END_ERROR);

// State machine stuff

// Forward declarations
struct DriveIdleParams;
struct DriveOpControlParams;
struct DriveMttParams;
struct DriveTurnToAngleParams;
struct DriveTurnToTargetParams;
struct DriveFlattenParams;

#define DRIVE_STATE_TYPES DriveIdleParams, DriveOpControlParams, DriveMttParams, DriveTurnToAngleParams, DriveTurnToTargetParams, DriveFlattenParams

#define DRIVE_STATE_TYPES_VARIANT std::variant<DRIVE_STATE_TYPES>

struct DriveIdleParams{
  const char* getName();
  void handle();
  void handleStateChange(DRIVE_STATE_TYPES_VARIANT prev_state);
};

struct DriveOpControlParams{
  const char* getName();
  void handle();
  void handleStateChange(DRIVE_STATE_TYPES_VARIANT prev_state);
};

struct DriveMttParams{
  Vector target;
  E_Brake_Modes brake_mode = E_Brake_Modes::brake;
  double max_power;
  double end_error_x;
  double exit_power;
  E_Robot_Sides robot_side = E_Robot_Sides::automatic;

  DriveMttParams(Vector target, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double max_power = MAX_DRIVE_POWER, double end_error_x = 1.0, double exit_power = 0.0, E_Robot_Sides robot_side = E_Robot_Sides::automatic);

  const char* getName();
  void handle();
  void handleStateChange(DRIVE_STATE_TYPES_VARIANT prev_state);
};

struct DriveTurnToAngleParams{
  double angle;
  E_Brake_Modes brake_mode = E_Brake_Modes::brake;
  double end_error;
  double max_power;
  
  DriveTurnToAngleParams(double angle, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = TURNING_END_ERROR, double max_power = MAX_TURNING_POWER);

  const char* getName();
  void handle();
  void handleStateChange(DRIVE_STATE_TYPES_VARIANT prev_state);
};

struct DriveTurnToTargetParams{
  Vector target;
  double offset = 0.0;
  bool reverse = false;
  E_Brake_Modes brake_mode = E_Brake_Modes::brake;
  double end_error;
  double max_power;
  
  DriveTurnToTargetParams(Vector target, double offset = 0.0, bool reverse = false, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = TURNING_END_ERROR, double max_power = MAX_TURNING_POWER);

  const char* getName();
  void handle();
  void handleStateChange(DRIVE_STATE_TYPES_VARIANT prev_state);
};

struct DriveFlattenParams{
  const char* getName();
  void  handle();
  void handleStateChange(DRIVE_STATE_TYPES_VARIANT prev_state);
};

extern Machine<DRIVE_STATE_TYPES> drive;