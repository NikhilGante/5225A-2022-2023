#pragma once
#include <array>

class Timer;

extern Timer op_control_timer;
constexpr int polynomial(int x, double curvature);
constexpr int poly_min_pow(int x, double curvature);

class CustomDrive{
  private:
    std::array<int, 255> lookup_table;
    // curve functions
    constexpr int polynomial(int x) const;  // transforms a linear input to a polynomially tranformed output
    constexpr int exponential(int x) const;  // transforms a linear input to a exponentially tranformed output
    constexpr int& lookup(unsigned char x);  // returns transformed value from lookup table, taking in an x input
    void fillLookupTable(); // transforms linear mapping to exponential mapping for a singular axis
    
    double curvature;

  public:
    constexpr CustomDrive(double curvature); // constructor
    constexpr int lookup(unsigned char x) const;  // returns transformed value from lookup table, taking in an x input
};

// moves the drivebase given 2 local powers: y and angle
void moveDrive(double y, double a);
void moveDriveSide(double l, double r); // sends powers to each side of the drivebase
void driveBrake(); // holds all the drivebase motors
void driveHandleInput(); // takes input from the joysticks and moves the drivebase

void driverPractice();  // Initializes state and runs driver code logic in loop

void shiftTrans(bool state);

void driveSpeedTest();