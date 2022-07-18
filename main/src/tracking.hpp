#pragma once
#include "main.h"
#include "Libraries/geometry.hpp"

enum class brake_modes{
  none, // the robot will keep going at whatever speed it was already going at
  coast,  // drivebase motors will turn off
  brake // the drivebase motors will hold
};

class Tracking{

public:
  double l_vel, r_vel, b_vel; // velocities of each of the tracking wheel in inches/sec
  Position g_pos{0.0, 0.0, 0.0};
  Position g_vel;


};
extern Tracking tracking;

void tracking_update();
void moveToTarget(Position target, brake_modes brake_mode = brake_modes::brake, double max_power = 127, double exit_power = 0.0, bool overshoot = false, double end_error_d = 0.5, double end_error_a = 3.0);
