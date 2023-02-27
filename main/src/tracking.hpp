#pragma once
#include "main.h"
#include "Libraries/geometry.hpp"
#include "Libraries/state.hpp"
#include "Libraries/task.hpp"

static constexpr double MAX_TURNING_POWER = 127;
static constexpr double TURNING_END_ERROR = 1.5;
static constexpr int MAX_DRIVE_POWER = 127;
static constexpr double TICKS_TO_INCHES = 2.75*std::numbers::pi/36000;
static constexpr double MM_TO_IN = okapi::mmToInch;
static constexpr double HALF_DRIVEBASE_WIDTH = 13.5/2;
constexpr double LEFT_DIST_OFFSET = 0.5;  // How far in the left sensor is from left edge
constexpr double RIGHT_DIST_OFFSET = 0.5;  // How far in the right sensor is from right edge
constexpr double BACK_DIST_OFFSET = 6.75;  // How far in the ultrasonic is from back edge
// constexpr double BACK_EDGE_DIST = 9.0;  // How far back edge is from tracking centre


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


double getDistL();  // Gets tracking centre's position from wall on left
double getDistR();  // Gets tracking centre's position from wall on right
double getDistBack(); // Gets tracking centre's position from back wall


class Tracking{
  private:
    _Task task{"Tracking Update"};
    Mutex pos_mutex; // locks g_pos
    Position g_pos{};

    // Movement related fields
    Position power; // power to apply to the drive motors
  
  public:
    static constexpr double min_move_power_y = 25, min_move_power_a = 35;

    // Odometry related variables
    double l_vel, r_vel, b_vel; // Velocities of each of the tracking wheel in inches/sec
    Position g_vel;
    std::atomic<double> drive_error; // How far the robot is from it's target

    void init(Position initial_pos = {});
    void waitForComplete(); // Waits until the motion completes
    void waitForDistance(double distance); // Waits until the robot is within a certain distance from it's target
    void reset(Position pos = {0.0, 0.0, 0.0}); // Resets the global tracking position to pos
    Position getPos(); // Getter for g_pos. Should probably wrap in a mutex
};
extern Tracking tracking;

void trackingUpdate();
void handleBrake(E_Brake_Modes brake_mode); // Brakes depending on type of brake mode passed in

// Wrapper functions for drive states (motion algorithms)
void moveToTargetSync (Vector target, E_Brake_Modes brake_mode = E_Brake_Modes::brake, uint8_t max_power = MAX_DRIVE_POWER, double end_error_x = 1.0, E_Robot_Sides robot_side = E_Robot_Sides::automatic);
void moveToTargetAsync(Vector target, E_Brake_Modes brake_mode = E_Brake_Modes::brake, uint8_t max_power = MAX_DRIVE_POWER, double end_error_x = 1.0, E_Robot_Sides robot_side = E_Robot_Sides::automatic);

void turnToAngleSync (double angle, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = TURNING_END_ERROR, double max_power = MAX_TURNING_POWER);
void turnToAngleAsync(double angle, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = TURNING_END_ERROR, double max_power = MAX_TURNING_POWER);

void turnToTargetSync (Vector target, double offset = 0.0, bool reverse = false, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = TURNING_END_ERROR, double max_power = MAX_TURNING_POWER);
void turnToTargetAsync(Vector target, double offset = 0.0, bool reverse = false, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = TURNING_END_ERROR, double max_power = MAX_TURNING_POWER);

void flattenAgainstWallSync ();
void flattenAgainstWallAsync();

// Takes a function that returns an angle in radians
void turnToAngleInternal(std::function<double()> getAngleFunc, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = TURNING_END_ERROR, double max_power = MAX_TURNING_POWER);
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
  uint8_t max_power;
  double end_error_x;
  E_Robot_Sides robot_side = E_Robot_Sides::automatic;

  DriveMttParams(Vector target, E_Brake_Modes brake_mode = E_Brake_Modes::brake, uint8_t max_power = MAX_DRIVE_POWER, double end_error_x = 1.0, E_Robot_Sides robot_side = E_Robot_Sides::automatic);

  inline static const std::string name = "DriveMoveToTarget";
  void handle();
  void handleStateChange(driveVariant prev_state);
};

struct DriveTurnToAngleParams{
  double angle;
  E_Brake_Modes brake_mode = E_Brake_Modes::brake;
  double end_error;
  double max_power;
  
  DriveTurnToAngleParams(double angle, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = TURNING_END_ERROR, double max_power = MAX_TURNING_POWER);

  inline static const std::string name = "DriveTurnToAngle";
  void handle();
  void handleStateChange(driveVariant prev_state);
};

struct DriveTurnToTargetParams{
  Vector target;
  double offset = 0.0;
  bool reverse = false;
  E_Brake_Modes brake_mode = E_Brake_Modes::brake;
  double end_error;
  double max_power;
  
  DriveTurnToTargetParams(Vector target, double offset = 0.0, bool reverse = false, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = TURNING_END_ERROR, double max_power = MAX_TURNING_POWER);

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