#include "drive.hpp"
#include "Libraries/controller.hpp"

double angle_curvature = 2.0;

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
  front_l.move_relative(0, 200);
  front_r.move_relative(0, 200);
  back_l.move_relative(0, 200);
  back_r.move_relative(0, 200);
  centre_l.move_relative(0, 200);
  centre_r.move_relative(0, 200);
}

void resetDrive(){TO_IMPLEMENT}
void flattenToWall(){TO_IMPLEMENT};

double l_power_last, r_power_last;
const double slew_val = 3;
void driveHandleInput(){
  int power_x, power_y, power_a;
  // double l_power, r_power;

  power_y = master.get_analog(ANALOG_LEFT_Y);
  power_a = 0.7 * polynomial(master.get_analog(ANALOG_RIGHT_X), angle_curvature);

  if(abs(power_y) < 7) power_y = 0;
  if(abs(power_a) < 7) power_a = 0;
  // if(power_y < -30){
  //   power_y = -30;
  //   // master.rumble("-");
  // }
  // if(abs(power_a) > 65) power_a = sgn(power_a) * 65;


  // l_power = power_y + power_a;
  // r_power = power_y - power_a;

  // if(fabs(l_power - l_power_last) > slew_val) l_power = l_power_last + slew_val*sgn(l_power - l_power_last);
  // if(fabs(r_power - r_power_last) > slew_val) r_power = r_power_last + slew_val*sgn(r_power - r_power_last);

  // // printf("%lf %lf %lf %lf\n", l_power, l_power_last, r_power, r_power_last);
  // moveDriveSide(l_power, r_power);
  // l_power_last = l_power,  r_power_last = r_power;
  
  lcd::print(5, "L| f:%.lf c:.%lf, b:%lf", front_l.get_temperature(), centre_l.get_temperature(), back_l.get_temperature());
  lcd::print(6, "R| f:%.lf c:.%lf, b:%lf", front_r.get_temperature(), centre_r.get_temperature(), back_r.get_temperature());


  if(front_l.get_temperature() >= 50 || centre_l.get_temperature() >= 50 || back_l.get_temperature() >= 50 || front_r.get_temperature() >= 50 || centre_r.get_temperature() >= 50 || back_r.get_temperature() >= 50){
    moveDrive(0, 0);
    master.rumble("----------");
    WAIT_UNTIL(false);
  } 
  moveDrive(power_y, power_a);
}