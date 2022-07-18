#pragma once
#include "main.h"
#include "Libraries/geometry.hpp"

class Tracking{

public:
  double l_vel, r_vel, b_vel; // velocities of each of the tracking wheel in inches/sec
  Position g_position;
  Position g_velocity;


  void update();

};

extern Tracking tracking;


