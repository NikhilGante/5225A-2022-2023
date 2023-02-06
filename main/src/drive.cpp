#include "drive.hpp"
#include "config.hpp"
#include "tracking.hpp"
#include "Libraries/controller.hpp"
#include "Libraries/motor.hpp"
#include "Libraries/piston.hpp"
#include "Subsystems/intake.hpp"
#include "Subsystems/shooter.hpp"

static constexpr double drive_curvature = 1.0;
static constexpr double angle_curvature = 2.0;

constexpr int polynomial(int x, double curvature){
  const double n = curvature * 0.2 + 1; // scales curvature value to match expo function
  return std::round(std::pow(127, 1 - n) * std::pow(std::abs(x), n) * sgn(x));
}

constexpr int poly_min_pow(int x, double curvature){
  double n = curvature * 0.2 + 1; // scales curvature value to match expo function
  if(std::abs(x) < _Controller::deadzone) return 0;
  return round(sgn(x) * ((127 - tracking.min_move_power_a)/pow(127 - _Controller::deadzone, n) * pow(abs(x) - _Controller::deadzone, n) + tracking.min_move_power_a));
}
// private methods
constexpr int CustomDrive::polynomial (int x) const {return ::polynomial(x, curvature);}
constexpr int CustomDrive::exponential(int x) const {
  const double n = curvature;
  return std::round(exp(0.002 * n * (std::abs(x) - 127)) * x);
}

// CustomDrive constructor
constexpr CustomDrive::CustomDrive(double curvature): curvature(curvature){}

void CustomDrive::fillLookupTable(){
  for (short x = -127; x < 128; x++){ // fills lookup table with values from appropriate function
    lookup(x) = exponential(x);
    // UNCOMMENT THESE FOR DEBUGGING, comment for performance
    // printf2("%d, %d", x, lookup(x));
    // delay(1);
  }
}

constexpr int& CustomDrive::lookup(unsigned char x)       {return lookup_table[x];}
constexpr int  CustomDrive::lookup(unsigned char x) const {return lookup_table[x];}

void moveDrive(double y, double a) {moveDriveSide(y+a, y-a);}

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

Timer curve_print_timer{"Curve Print"};
Timer backwards_timer{"Backwards"};
constexpr int slew = 5;
bool backwards = false;
bool last_backwards = false;

void driveHandleInput(){
  double power_y = polynomial(master.getAnalog(ANALOG_LEFT_Y), drive_curvature);
  double power_a = 0.6 * polynomial(master.getAnalog(ANALOG_RIGHT_X), angle_curvature);
 
  if(std::abs(power_y) < _Controller::deadzone) power_y = 0;
 
  backwards = power_y < 0;
  // if(backwards && !last_backwards){
  //   backwards_timer.reset();
  // }
  // if(!backwards && last_backwards){
  //   backwards_timer.reset(false);
  // }
  // last_backwards = backwards;
  // if(backwards_timer.getTime() > 800){
  //   power_y = 0;
  // }

  if(std::abs(power_y) < 7) power_y = 0;
  if(std::abs(power_a) < 7) power_a = 0;

  for(_Motor* motor: _Motor::getList()){
    if(motor->getTemperature() >= 50){
      moveDrive(0, 0);
      master.rumble("----------");
      WAIT_UNTIL(false);
    }
  }

  if(master.getNewDigital(transToggleBtn)) trans_p.toggleState();
  moveDrive(power_y, power_a);
}

int power_sgn = 0; // Equals -1 if power < -dz, 1 if power > dz, and 0 otherwise (used to check for sudden braking by driver)
int last_power_sgn = 0; // sgn of power last cycle
int prev_power_sgn = 0; // sgn of power before it was 0 (need to think of better name for this var)

double l_power_last, r_power_last;
constexpr double slew_val = 3;
void driveHandleInputProg(){
  int power_x, power_y, power_a;

  power_y = master.getAnalog(ANALOG_LEFT_Y);
  power_a = 0.7 * polynomial(master.getAnalog(ANALOG_RIGHT_X), angle_curvature);

  if(std::abs(power_y) < _Controller::deadzone) power_y = 0;
  if(std::abs(power_a) < _Controller::deadzone) power_a = 0;
  // if(power_y < -30){
  //   power_y = -30;
  //   // master.rumble("-");
  // }
  if(std::abs(power_a) > 65) power_a = sgn(power_a) * 65;


  double l_power = power_y + power_a;
  double r_power = power_y - power_a;

  if(std::abs(l_power - l_power_last) > slew_val) l_power = l_power_last + slew_val*sgn(l_power - l_power_last);
  if(std::abs(r_power - r_power_last) > slew_val) r_power = r_power_last + slew_val*sgn(r_power - r_power_last);

  // driver_log("%lf %lf %lf %lf", l_power, l_power_last, r_power, r_power_last);
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

  Timer endgame_click_timer{"endgame_timer"};
  endgame_click_timer.reset(false);
  bool endgame_dbl_click = false;
  // driveBrake();
  // drive.changeState(DriveIdleParams{});
	while(true){

    if(endgame_click_timer.getTime() > 300){
      driver_log("timer reset: %lld", endgame_click_timer.getTime());
      endgame_click_timer.reset(false);
      endgame_dbl_click = false;
      driver_log("SHOULD BE FALSE dbl_click: %d", endgame_dbl_click);

    }
    if(master.getNewDigital(endgameBtn)){
      driver_log("PRESSED | timer reset: %lld", endgame_click_timer.getTime());
      driver_log("dbl_click: %d", endgame_dbl_click);
      if(endgame_dbl_click) {
        endgame_s_p.setState(HIGH);
      }
      else endgame_dbl_click = true;
      endgame_click_timer.reset();
    }

		// driveHandleInput();
		shooterHandleInput();
		intakeHandleInput();
		if((master.getNewDigital(DIGITAL_UP) || partner.getNewDigital(DIGITAL_UP)) && g_mag_disc_count < 3)	g_mag_disc_count++;
		if((master.getNewDigital(DIGITAL_DOWN) || partner.getNewDigital(DIGITAL_DOWN)) && g_mag_disc_count > 0)	g_mag_disc_count--; 


		if(disc_count_print.getTime() > 100){
			master.print(0, "disc count: %d", g_mag_disc_count.load());
			// partner.print(0,0, "disc count: %d", g_mag_disc_count.load());
			disc_count_print.reset();
		}

		if(angle_override_print.getTime() > 100){
			angle_override_print.reset();
			if (angleOverride) master.print(1, "Override");
			else master.print(1, "Automatic");
		}

    for(_Motor* motor: _Motor::getList()){
      if(motor->getTemperature() >= 50){
        moveDrive(0, 0);
        master.rumble("----------");
        WAIT_UNTIL(false);
      }
    }
		delay(10);
	}
}