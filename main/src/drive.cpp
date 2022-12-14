#include "drive.hpp"
#include "config.hpp"
#include "util.hpp"
#include "Libraries/controller.hpp"
#include "Libraries/motor.hpp"
#include "Libraries/timer.hpp"

#include <cmath>

double angle_curvature = 2.0;

int polynomial(int x, double curvature){
  double n = curvature * 0.2 + 1; // scales curvature value to match expo function
  return std::round(std::pow(127, 1 - n) * std::pow(std::abs(x), n) * sgn(x));
}
// private methods
int CustomDrive::polynomial(int x){
  double n = curvature * 0.2 + 1; // scales curvature value to match expo function
  return std::round(std::pow(127, 1 - n) * std::pow(std::abs(x), n) * sgn(x));
}
int CustomDrive::exponential(int x){
  double n = curvature;
  return std::round(exp(0.002 * n * (std::abs(x) - 127)) * x);
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

void moveDrive(double y, double a){
  front_l.move(y+a);
  centre_l.move(y+a);
  back_l.move(y+a);
  front_r.move(y-a);
  centre_r.move(y-a);
  back_r.move(y-a);
}

void moveDriveSide(double l, double r){
  front_l.move(l);
  centre_l.move(l);
  back_l.move(l);
  front_r.move(r);
  centre_r.move(r);
  back_r.move(r);
}

void driveBrake(){
  front_l.brake();
  front_r.brake();
  back_l.brake();
  back_r.brake();
  centre_l.brake();
  centre_r.brake();
}

void resetDrive(){TO_IMPLEMENT}
void flattenToWall(){TO_IMPLEMENT};

double l_power_last, r_power_last;
const double slew_val = 3;
Timer curve_print_timer{"curve_print_timer"};
void driveHandleInput(){
  int power_x, power_y, power_a;
  // double l_power, r_power;
  if(master.get_digital_new_press(DIGITAL_UP)){
    angle_curvature += 0.1;
    master.print(0,0, "curvature: %.2lf", angle_curvature);
  }
  if(master.get_digital_new_press(DIGITAL_DOWN)){
    angle_curvature -= 0.1;
    master.print(0,0, "curvature: %.2lf", angle_curvature);
  }

  power_y = master.get_analog(ANALOG_LEFT_Y);
  power_a = 0.7 * polynomial(master.get_analog(ANALOG_RIGHT_X), angle_curvature);

  if(std::abs(power_y) < 7) power_y = 0;
  if(std::abs(power_a) < 7) power_a = 0;
  // if(power_y < -30){
  //   power_y = -30;
  //   // master.rumble("-");
  // }
  // if(std::abs(power_a) > 65) power_a = sgn(power_a) * 65;


  // l_power = power_y + power_a;
  // r_power = power_y - power_a;

  // if(fstd::abs(l_power - l_power_last) > slew_val) l_power = l_power_last + slew_val*sgn(l_power - l_power_last);
  // if(fstd::abs(r_power - r_power_last) > slew_val) r_power = r_power_last + slew_val*sgn(r_power - r_power_last);

  // // printf("%lf %lf %lf %lf\n", l_power, l_power_last, r_power, r_power_last);
  // moveDriveSide(l_power, r_power);
  // l_power_last = l_power,  r_power_last = r_power;
  for(_Motor* motor: _Motor::getList()){
    if(motor->getTemperature() >= 50){
      moveDrive(0, 0);
      master.rumble("----------");
      WAIT_UNTIL(false);
    }
  }

  moveDrive(power_y, power_a);
}