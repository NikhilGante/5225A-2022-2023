#pragma once
#include "main.h"
#include <cmath>
#include "config.hpp"
#include "util.hpp"
using namespace pros;


extern double gA, gX, gY;
extern double distance_lr, distance_b;



// Tracking Things
#define TICKS_TO_INCHES (2.75*M_PI)/360

void tracking();



void moveXDrive(double x, double y, double a);
void moveDrive(double lMove, double rMov);

enum class E_Brake_Modes{
  none, // the robot will keep going at whatever speed it was already going at
  coast,  // drivebase motors will turn off
  brake // the drivebase motors will hold
};

// ---------------- Motion Algorithms ----------------

void xDriveMTT(double x, double y, double a, E_Brake_Modes brake_mode, double end_error, double max_speed = 60, double min_speed = 40, bool anglePriority = false, double angle_min = 40);
void xDriveTTA(double a, E_Brake_Modes brake_mode, double end_error);
void xDriveTTT(double x, double y, E_Brake_Modes brake_mode, double end_error);
void xDriveARC(double x, double y, double px, double py, int intervals, E_Brake_Modes brake_mode, double end_error);


void dropWheelDriveMTT(double x, double y, E_Brake_Modes brake_mode, double end_error, double back = false);
void dropWheelDriveTTA(double a, E_Brake_Modes brake_mode, double end_error);
void dropWheelDriveTTT(double x, double y, E_Brake_Modes brake_mode, double end_error);


