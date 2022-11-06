#pragma once
#include "main.h"
#include <cmath>
#include "config.hpp"
using namespace pros;


extern double gA, gX, gY;
extern double distance_lr, distance_b;



// Tracking Things
#define TICKS_TO_INCHES (2.75*M_PI)/360

void tracking();



void moveDrive(double x, double y, double a);

enum class E_Brake_Modes{
  none, // the robot will keep going at whatever speed it was already going at
  coast,  // drivebase motors will turn off
  brake // the drivebase motors will hold
};

// ---------------- Motion Algorithms ----------------

void xDriveMTT(double x, double y, double a, E_Brake_Modes brake_mode, double end_error);
void xDriveTTA(double a, E_Brake_Modes brake_mode, double end_error);



int sgn(double x);