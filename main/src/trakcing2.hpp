// #pragma once
// #include "Libraries/logging.hpp"
// #include "main.h"
// #include "Libraries/geometry.hpp"

// enum class E_Brake_Modes{
//   none, // the robot will keep going at whatever speed it was already going at
//   coast,  // drivebase motors will turn off
//   brake // the drivebase motors will hold
// };

// enum class E_Robot_Sides{
//   front,
//   back,
//   automatic // automatically deduce which side
// };

// class Tracking{
  
// public:
//   const double min_move_power_y = 25.0, min_move_power_a = 30.0;
//   // odometry related variables
//   double l_vel, r_vel, b_vel; // velocities of each of the tracking wheel in inches/sec
//   Position g_pos{};
//   Position g_vel;

//   // movement related fields
//   Position power; // power to apply to the drive motors
//   double drive_error; // how far the robot is from it's target 
//   void waitForComplete(); // waits until the motion completes
//   void waitForDistance(double distance); // waits until the robot is within a certain distance from it's target

//   // helper methods for motion algorithms
//   void supplyMinPower(const Position& error);  // if any axis has less than the min power, give it min power 
//   void scalePowers(uint8_t max_power, uint8_t min_angle_power);  // scales powers so that the drivebase doesn't travel faster than max_power
//   // ensures drivebase gets enough angle power AFTER scaling
//   void guaranteeAnglePower(uint8_t min_angle_power, uint8_t pre_scaled_power_a, uint8_t max_power);

//   // // moveToTarget is declared a friend so it can access private tracking properties
//   // friend void moveToTarget(Position, E_Brake_Modes, double max_power, double exit_power, bool overshoot, double end_error_d, double end_error_a);

// };
// extern Tracking tracking;

// void trackingUpdate();
// void handleBrake(E_Brake_Modes brake_mode); // Brakes depending on type of brake mode passed in
// void moveToTarget(Vector target, E_Brake_Modes brake_mode = E_Brake_Modes::brake, uint8_t max_power = 127, double end_error_x = 1.0, E_Robot_Sides robot_side = E_Robot_Sides::automatic);
// void turnToAngle(double angle, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = 1.5);