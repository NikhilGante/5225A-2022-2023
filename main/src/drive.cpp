#include "drive.hpp"
#include "config.hpp"
#include "tracking.hpp"
#include "util.hpp"
#include "Libraries/controller.hpp"
#include "Libraries/motor.hpp"
#include "Libraries/piston.hpp"
#include "Libraries/timer.hpp"
#include "Subsystems/intake.hpp"
#include "Subsystems/shooter.hpp"

#include <cmath>

static constexpr double drive_curvature = 1.0;
static constexpr double angle_curvature = 2.0;

static constexpr int deadzone = 7;

int polynomial(int x, double curvature){
  double n = curvature * 0.2 + 1; // scales curvature value to match expo function
  return std::round(std::pow(127, 1 - n) * std::pow(std::abs(x), n) * sgn(x));
}

int poly_min_pow(int x, double curvature){
  double n = curvature * 0.2 + 1; // scales curvature value to match expo function
  if(abs(x) < deadzone) return 0;
  return round(sgn(x) * ((127 - tracking.min_move_power_a)/pow(127 - deadzone, n) * pow(abs(x) - deadzone, n) + tracking.min_move_power_a));
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

Timer curve_print_timer{"curve_print_timer"};
int slew = 5;
Timer backwards_timer{"backwards_timer"};
bool backwards = false;
bool last_backwards = false;

void driveHandleInput(){
  double power_y = polynomial(master.get_analog(ANALOG_LEFT_Y), drive_curvature);
  double power_a = 0.6 * polynomial(master.get_analog(ANALOG_RIGHT_X), angle_curvature);
 
  if(fabs(power_y) < deadzone) power_y = 0;
 
  backwards = power_y < 0;
  if(backwards && !last_backwards){
    backwards_timer.reset();
  }
  if(!backwards && last_backwards){
    backwards_timer.reset(false);
  }
  last_backwards = backwards;
  if(backwards_timer.getTime() > 800){
    power_y = 0;
  }

  if(std::abs(power_y) < 7) power_y = 0;
  if(std::abs(power_a) < 7) power_a = 0;

  for(_Motor* motor: _Motor::getList()){
    if(motor->getTemperature() >= 50){
      moveDrive(0, 0);
      master.rumble("----------");
      WAIT_UNTIL(false);
    }
  }

  if(master.get_digital_new_press(transToggleBtn)) trans_p.toggleState();
  moveDrive(power_y, power_a);
}

constexpr int dz = 7; // Deadzone for joystick

int power_sgn = 0; // Equals -1 if power < -dz, 1 if power > dz, and 0 otherwise (used to check for sudden braking by driver)
int last_power_sgn = 0; // sgn of power last cycle
int prev_power_sgn = 0; // sgn of power before it was 0 (need to think of better name for this var)

Timer zero_power_timer{"zero_power_timer"};
// void driveHandleInput(){
//   int power_x, power_y, power_a;

//   power_y = master.get_analog(ANALOG_LEFT_Y);
//   power_a = 0.7 * polynomial(master.get_analog(ANALOG_RIGHT_X), angle_curvature);

//   if(std::abs(power_y) < dz) power_y = 0;
//   if(std::abs(power_a) < dz) power_a = 0;


//   if(power_y > dz)  power_sgn = 1;
//   else if(power_y < dz)  power_sgn = -1;
//   else power_sgn = 0;

//   // if power sgn just became 0
//   if (power_sgn == 0 && last_power_sgn != 0)  zero_power_timer.reset();

//   // if power sgn just became -1 or 1
//   if (power_sgn != 0 && last_power_sgn == 0){
//     if(power_sgn != prev_power_sgn && zero_power_timer.getTime() < 100)  master.rumble("-"); // notifies driver that they just harshly applied the brakes
//     prev_power_sgn = power_sgn; 
//   }

//   last_power_sgn = power_sgn;

//   lcd::print(4, "intk: %.lf", intake_m.get_temperature());
//   lcd::print(5, "L| f:%.lf c:%.lf, b:%.lf", front_l.get_temperature(), centre_l.get_temperature(), back_l.get_temperature());
//   lcd::print(6, "R| f:%.lf c:%.lf, b:%.lf", front_r.get_temperature(), centre_r.get_temperature(), back_r.get_temperature());


//   if(front_l.get_temperature() >= 50 || centre_l.get_temperature() >= 50 || back_l.get_temperature() >= 50 || front_r.get_temperature() >= 50 || centre_r.get_temperature() >= 50 || back_r.get_temperature() >= 50 || intake_m.get_temperature() >= 50){
//     moveDrive(0, 0);
//     master.rumble("----------");
//     WAIT_UNTIL(false);
//   } 
//   moveDrive(power_y, power_a);
// }

double l_power_last, r_power_last;
constexpr double slew_val = 3;
void driveHandleInputProg(){
  int power_x, power_y, power_a;

  power_y = master.get_analog(ANALOG_LEFT_Y);
  power_a = 0.7 * polynomial(master.get_analog(ANALOG_RIGHT_X), angle_curvature);

  if(std::abs(power_y) < dz) power_y = 0;
  if(std::abs(power_a) < dz) power_a = 0;
  // if(power_y < -30){
  //   power_y = -30;
  //   // master.rumble("-");
  // }
  if(std::abs(power_a) > 65) power_a = sgn(power_a) * 65;


  double l_power = power_y + power_a;
  double r_power = power_y - power_a;

  if(std::abs(l_power - l_power_last) > slew_val) l_power = l_power_last + slew_val*sgn(l_power - l_power_last);
  if(std::abs(r_power - r_power_last) > slew_val) r_power = r_power_last + slew_val*sgn(r_power - r_power_last);

  // printf("%lf %lf %lf %lf\n", l_power, l_power_last, r_power, r_power_last);
  moveDriveSide(l_power, r_power);
  l_power_last = l_power,  r_power_last = r_power;

  for(_Motor* motor: _Motor::getList()){
    if(motor->getTemperature() >= 50){
      moveDrive(0, 0);
      master.rumble("----------");
      WAIT_UNTIL(false);
    }
  }

}

void driverPractice(){  // Initializes state and runs driver code logic in loop
  Timer disc_count_print{"disc_count_print"};
	Timer angle_override_print{"angle_override_print"};
	master.clear();

  // Initialises states of subsystems
	drive.changeState(DriveOpControlParams{});
  setFlywheelVel(barrier_rpm);
  intakeOn();
  angleOverride = false;
	while(true){

		// driveHandleInput();
		shooterHandleInput();
		intakeHandleInput();
		if((master.get_digital_new_press(DIGITAL_UP) || partner.get_digital_new_press(DIGITAL_UP)) && g_mag_disc_count < 3)	g_mag_disc_count++;
		if((master.get_digital_new_press(DIGITAL_DOWN) || partner.get_digital_new_press(DIGITAL_DOWN)) && g_mag_disc_count > 0)	g_mag_disc_count--; 


		if(disc_count_print.getTime() > 100){
			master.print(0,0, "disc count: %d  ", g_mag_disc_count.load());
			// partner.print(0,0, "disc count: %d  ", g_mag_disc_count.load());
			disc_count_print.reset();
		}

		if(angle_override_print.getTime() > 100){
			angle_override_print.reset();
			if (angleOverride) master.print(1, 0, "Override");
			else master.print(1, 0, "Automatic");
		}

		// if(centre_l.get_temperature() >= 50 || centre_r.get_temperature() >= 50 || intake_m.get_temperature() >= 50 || flywheel_m.get_temperature() >= 50){
		// 	break;
		// }
		delay(10);
	}
}