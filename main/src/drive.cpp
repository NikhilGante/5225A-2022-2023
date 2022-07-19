#include "drive.hpp"
#include <cmath>

double angle_curvature = 0.0;

int polynomial(int x, double curvature){
  double n = curvature * 0.2 + 1; // scales curvature value to match expo function
  return round(pow(127, 1 - n) * pow(abs(x), n) * sgn(x));
}
// private methods
int CustomDrive::polynomial(int x){
  double n = curvature * 0.2 + 1; // scales curvature value to match expo function
  return round(pow(127, 1 - n) * pow(abs(x), n) * sgn(x));
}
int CustomDrive::exponential(int x){
  double n = curvature;
  return round(exp(0.002 * n * (abs(x) - 127)) * x);
}

// CustomDrive constructor
CustomDrive::CustomDrive(double curvature): curvature(curvature){}

void CustomDrive::fillLookupTable(){
  for (short x = -127; x < 128; x++){ // fills lookup table with values from appropriate function
    lookup_table[(unsigned char)x] = exponential(x);
    // UNCOMMENT THESE FOR DEBUGGING, comment for performance
    // printf2("%d, %d", x, lookup_table[(unsigned char)x]);
    // delay(1);
  }
}

int CustomDrive::lookup(int x){
  return lookup_table[(unsigned char)x];
}

void moveDrive(double x, double y, double a){
	front_l.move(x + y + a);
  front_r.move(-x + y - a);
  back_l.move(-x + y + a);
  back_r.move(x + y - a);
}

void driveBrake(){
	front_l.move_relative(0, 200);
  front_r.move_relative(0, 200);
  back_l.move_relative(0, 200);
  back_r.move_relative(0, 200);
}

void handleInput(){
  int power_x, power_y, power_a;

  power_x = master.get_analog(ANALOG_LEFT_X);
  power_y = master.get_analog(ANALOG_LEFT_Y);
  power_a = polynomial(master.get_analog(ANALOG_RIGHT_X), angle_curvature);

  if(abs(power_x) < 7) power_x = 0;
  if(abs(power_y) < 7) power_y = 0;
  if(abs(power_a) < 7) power_a = 0;

  moveDrive(power_x, power_y, power_a);
}