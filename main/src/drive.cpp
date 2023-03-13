#include "drive.hpp"
#include "Libraries/logging.hpp"
#include "config.hpp"
#include "util.hpp"
#include "tracking.hpp"
#include "Devices/controller.hpp"
#include "Devices/motor.hpp"
#include "Devices/piston.hpp"
#include "Subsystems/intake.hpp"
#include "Subsystems/shooter.hpp"

Timer op_control_timer{"Op Control"};

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
constexpr int CustomDrive::polynomial (int x) const {return ::polynomial(x, curvature);}
constexpr int CustomDrive::exponential(int x) const {
  const double n = curvature;
  return std::round(std::exp(0.002 * n * (std::abs(x) - 127)) * x);
}

// CustomDrive constructor
constexpr CustomDrive::CustomDrive(double curvature): curvature(curvature){}

void CustomDrive::fillLookupTable(){
  for (short x = -127; x < 128; x++){ // fills lookup table with values from appropriate function
    lookup(x) = exponential(x);
    // UNCOMMENT THESE FOR DEBUGGING, comment for performance
    // driver_log2("%d, %d", x, lookup(x));
    // delay(1);
  }
}

constexpr int& CustomDrive::lookup(unsigned char x)       {return lookup_table.at(x);}
constexpr int  CustomDrive::lookup(unsigned char x) const {return lookup_table.at(x);}

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

Timer backwards_timer{"Backwards"};
constexpr int slew = 5;
bool backwards = false;
bool last_backwards = false;


void driveHandleInput(){
  double power_y = polynomial(master.getAnalog(ANALOG_LEFT_Y), drive_curvature);
  double power_a = 0.6 * polynomial(master.getAnalog(ANALOG_RIGHT_X), angle_curvature);

  _Controller::deadband(power_y);
 
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

  _Controller::deadband(power_y);
  _Controller::deadband(power_a);

  // Anti-tipping code
  // log("VEL %lf %lf\n", tracking.r_vel, power_y);
  // if(std::abs(tracking.r_vel) > 10 && sgn(tracking.r_vel) != sgn(power_y) && !power_a){
  //   power_y = sgn(power_y) * 1;

  // }

  if(inRangeExcl(std::abs(power_a), 7, tracking.min_move_power_a)) power_a = tracking.min_move_power_a * sgn(power_a); //Give min power to driver when turning

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

  _Controller::deadband(power_y);
  _Controller::deadband(power_a);
  // if(power_y < -30){
  //   power_y = -30;
  //   // master.rumble();
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
      device_log("CONTROLLER RUMBLING FROM LINE %d in file %s", __LINE__, __FILE__);
      WAIT_UNTIL(false);
    }
  }
}

void driverPractice(){  // Initializes state and runs driver code logic in loop
  op_control_timer.reset();
  Timer disc_count_print{"Disc Count Print", driver_log};
	Timer angle_override_print{"Angle Override Print", driver_log};
  Timer low_gear_buzz_timer{"Low Gear Buzz", driver_log};

	master.clear();

  // Initialises states of subsystems
	drive.changeState(DriveOpControlParams{});
  setFlywheelVel(barrier_rpm);
  intakeOn();
  angleOverride = false;

  Timer endgame_click_timer_left{"Left Endgame", driver_log};
  endgame_click_timer_left.reset(false);
  bool endgame_dbl_click_left = false;

  Timer endgame_click_timer_right{"Right Endgame", driver_log};
  endgame_click_timer_right.reset(false);
  bool endgame_dbl_click_right = false;
  // driveBrake();
  // drive.changeState(DriveIdleParams{});

	while(true){
    if(master.getNewDigital(transToggleBtn)) shiftTrans(!trans_p.getState());
  
    if(trans_p.getState() == LOW && low_gear_buzz_timer.getTime() > 800){  // Buzzes if in low gear for driver
      low_gear_buzz_timer.reset();
      master.rumble("-");
    }

    if(endgame_click_timer_left.getTime() > 300){
      endgame_click_timer_left.reset(false);
      endgame_dbl_click_left = false;
    }
    if(master.getNewDigital(endgameBtnLeft)){
      if(endgame_dbl_click_left) {
        driver_log("%lld | LEFT ENDGAME FIRED\n", op_control_timer.getTime());
        endgame_s_p.setState(HIGH);
      }
      else endgame_dbl_click_left = true;
      endgame_click_timer_left.reset();
    }

    if(endgame_click_timer_right.getTime() > 300){
      endgame_click_timer_right.reset(false);
      endgame_dbl_click_right = false;
    }
    if(master.getNewDigital(endgameBtnRight)){
      if(endgame_dbl_click_right) {
        driver_log("%lld | RIGHT ENDGAME FIRED\n", op_control_timer.getTime());
        endgame_d_p.setState(HIGH);
      }
      else endgame_dbl_click_right = true;
      endgame_click_timer_right.reset();
    }
    // if(master.getDigital(endgameBtnLeft) && master.getDigital(endgameBtnRight)){
    //   endgame_d_p.setState(HIGH);
    //   endgame_s_p.setState(HIGH);
    // }

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
        // moveDrive(0, 0);
        master.rumble();
        device_log("CONTROLLER RUMBLING FROM LINE %d in file %s", __LINE__, __FILE__);
        delay(50);
        // WAIT_UNTIL(false);
      }
    }
		delay(10);
	}
}

void shiftTrans(bool state){
  _Task trans_task;
  trans_task.start([=](){
    driver_log("%d Transmission started shifting into %s gear\n", millis(), state ? "HIGH" : "LOW");
    trans_p.setState(state);
    drive.changeState(DriveIdleParams{});
    drive.waitToReachState(DriveIdleParams{});
    moveDrive(0, 0);
    delay(100);
    drive.changeState(DriveOpControlParams{});
    driver_log("%d Transmission finished shifting into %s gear\n", millis(), state ? "HIGH" : "LOW");
  });
}