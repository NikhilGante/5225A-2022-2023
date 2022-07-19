#pragma once
#include "config.hpp"
#include "util.hpp"

int polynomial(int x, double curvature);
extern double angle_curvature;

class CustomDrive{
  int lookup_table[255];
  // curve functions
  int polynomial(int x);  // transforms a linear input to a polynomially tranformed output
  int exponential(int x);  // transforms a linear input to a exponentially tranformed output

public:
  double curvature;

  CustomDrive(double curvature); // constructor
  void fillLookupTable(); // transforms linear mapping to exponential mapping for a singular axis
  int lookup(int x);  // returns transformed value from lookup table, taking in an x input
};

// moves the drivebase given 3 local powers: x, y and angle
void moveDrive(double x, double y, double a);
void driveBrake(); // holds all the drivebase motors
void handleInput(); // takes input from the joysticks and moves the drivebase