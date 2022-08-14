#pragma once
#include "main.h"
#include "Libraries/geometry.hpp"

enum class E_Brake_Modes{
  none, // the robot will keep going at whatever speed it was already going at
  coast,  // drivebase motors will turn off
  brake // the drivebase motors will hold
};

class Tracking{
  const Position min_move_power{15.0, 15.0, 10.0};  // min power to move the drivebase in each axis
  
public:
  // odometry related variables
  double l_vel, r_vel, b_vel; // velocities of each of the tracking wheel in inches/sec
  Position g_pos{};
  Position g_vel;

  // movement related fields
  Position power; // power to apply to the drive motors
  double drive_error; // how far the robot is from it's target 
  void waitForComplete(); // waits until the motion completes
  void waitForDistance(double distance); // waits until the robot is within a certain distance from it's target

  // helper methods for motion algorithms
  void supplyMinPower(const Position& error);  // if any axis has less than the min power, give it min power 
  void scalePowers(uint8_t max_power, uint8_t min_angle_power);  // scales powers so that the drivebase doesn't travel faster than max_power
  // ensures drivebase gets enough angle power AFTER scaling
  void guaranteeAnglePower(uint8_t min_angle_power, uint8_t pre_scaled_power_a, uint8_t max_power);

  // // moveToTarget is declared a friend so it can access private tracking properties
  // friend void moveToTarget(Position, E_Brake_Modes, double max_power, double exit_power, bool overshoot, double end_error_d, double end_error_a);

};
extern Tracking tracking;

void TrackingUpdate();
void moveToTarget(Position target, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double max_power = 127.0, double min_angle_power = 0.0, double exit_power = 0.0, bool overshoot = false, double end_error_d = 0.5, double end_error_a = 5.0);
