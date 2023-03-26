#include "drive.hpp"
#include "config.hpp"
// #include "Subsystems/intake.hpp"
// #include "Subsystems/flywheel.hpp"
#include "tracking.hpp"
#include "Subsystems/shooter.hpp"
#include <cmath>

Timer op_control_timer{"shoot_timer"};

double drive_curvature = 1.0;
double angle_curvature = 2.0;

const int deadzone = 7;

int polynomial(int x, double curvature){
  double n = curvature * 0.2 + 1; // scales curvature value to match expo function
  return round(pow(127, 1 - n) * pow(abs(x), n) * sgn(x));
}

int poly_min_pow(int x, double curvature){
  double n = curvature * 0.2 + 1; // scales curvature value to match expo function
  if(abs(x) < deadzone) return 0;
  return round(sgn(x) * ((127 - tracking.min_move_power_a)/pow(127 - deadzone, n) * pow(abs(x) - deadzone, n) + tracking.min_move_power_a));
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

Timer curve_print_timer{"curve_print_timer"};
int slew_val = 3;
int slew = 5;
Timer backwards_timer{"backwards_timer"};
bool backwards = false;
bool last_backwards = false;

double l_power_last, r_power_last;
void driveHandleInput(){
  double power_y = polynomial(master.get_analog(ANALOG_LEFT_Y), drive_curvature);
  double power_a = 0.6 * polynomial(master.get_analog(ANALOG_RIGHT_X), angle_curvature);
 
  if(fabs(power_y) < deadzone) power_y = 0;
 
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


  if(fabs(power_a) < deadzone) power_a = 0;
  else if (fabs(power_a) > 7 && fabs(power_a) < tracking.min_move_power_a){ // give min power to driver when turning
    power_a = tracking.min_move_power_a * sgn(power_a);
  }

  // Anti-tipping code
  // log("VEL %lf %lf\n", tracking.r_vel, power_y);
  // if(fabs(tracking.r_vel) > 10 && sgn(tracking.r_vel) != sgn(power_y) && !power_a){
  //   power_y = sgn(power_y) * 1;

  // }

  moveDrive(power_y, power_a);

  lcd::print(5, "L| f:%.lf c:.%lf, b:%lf", front_l.get_temperature(), centre_l.get_temperature(), back_l.get_temperature());
  lcd::print(6, "R| f:%.lf c:.%lf, b:%lf", front_r.get_temperature(), centre_r.get_temperature(), back_r.get_temperature());
  lcd::print(7, "intk:%.lf flywheel:%.lf", intake_m.get_temperature(), flywheel_m.get_temperature());

}


void driveHandleInputProg(){
  int power_x, power_y, power_a;

  power_y = master.get_analog(ANALOG_LEFT_Y);
  power_a = 0.7 * polynomial(master.get_analog(ANALOG_RIGHT_X), angle_curvature);

  if(abs(power_y) < deadzone) power_y = 0;
  if(abs(power_a) < deadzone) power_a = 0;
  // if(power_y < -30){
  //   power_y = -30;
  //   // master.rumble("-");
  // }
  if(abs(power_a) > 65) power_a = sgn(power_a) * 65;


  double l_power = power_y + power_a;
  double r_power = power_y - power_a;

  if(fabs(l_power - l_power_last) > slew_val) l_power = l_power_last + slew_val*sgn(l_power - l_power_last);
  if(fabs(r_power - r_power_last) > slew_val) r_power = r_power_last + slew_val*sgn(r_power - r_power_last);

  // printf("%lf %lf %lf %lf\n", l_power, l_power_last, r_power, r_power_last);
  moveDriveSide(l_power, r_power);
  l_power_last = l_power,  r_power_last = r_power;
  lcd::print(4, "intk: %.lf", intake_m.get_temperature());
  lcd::print(5, "L| f:%.lf c:%.lf, b:%.lf", front_l.get_temperature(), centre_l.get_temperature(), back_l.get_temperature());
  lcd::print(6, "R| f:%.lf c:%.lf, b:%.lf", front_r.get_temperature(), centre_r.get_temperature(), back_r.get_temperature());


  if(front_l.get_temperature() >= 50 || centre_l.get_temperature() >= 50 || back_l.get_temperature() >= 50 || front_r.get_temperature() >= 50 || centre_r.get_temperature() >= 50 || back_r.get_temperature() >= 50 || intake_m.get_temperature() >= 50){
    moveDrive(0, 0);
    master.rumble("----------");
    WAIT_UNTIL(false);
  } 
}

void driverPractice(){  // Initializes state and runs driver code logic in loop
  op_control_timer.reset();
	Timer angle_override_print{"angle_override_print"};
  Timer low_gear_buzz_timer{"low_gear_buzz_timer"};

	master.clear();

  // Initializes states of subsystems
	drive.changeState(DriveOpControlParams{});
  setFlywheelVel(barrier_rpm);
  intakeOn();
  angleOverride = false;

  Timer endgame_click_timer_left{"endgame_timer"};
  endgame_click_timer_left.reset(false);
  bool endgame_dbl_click_left = false;

  Timer endgame_click_timer_right{"endgame_timer"};
  endgame_click_timer_right.reset(false);
  bool endgame_dbl_click_right = false;
  // driveBrake();
  // drive.changeState(DriveIdleParams{});

  int g_mag_disc_count_lst = g_mag_disc_count;  // Mag count from last cycle

  master.print(0,0, "disc count: %d  ", g_mag_disc_count.load());

  uint32_t cycle_time = millis();
	while(true){
    master.updateButtons();


    if(master.get_digital_new_press(transToggleBtn))  shiftTrans(!trans_p.getState());
  
    if(trans_p.getState() == LOW && low_gear_buzz_timer.getTime() > 800){  // Buzzes if in low gear for driver
      low_gear_buzz_timer.reset();
      master.rumble("-");
    }

    if(endgame_click_timer_left.getTime() > 300){
      endgame_click_timer_left.reset(false);
      endgame_dbl_click_left = false;
    }
    if(master.get_digital_new_press(endgameBtnLeft)){
      if(endgame_dbl_click_left) {
        log("%lld | LEFT ENDGAME FIRED\n", op_control_timer.getTime());
        endgame_s_p.setState(HIGH);
      }
      else endgame_dbl_click_left = true;
      endgame_click_timer_left.reset();
    }

    if(endgame_click_timer_right.getTime() > 300){
      endgame_click_timer_right.reset(false);
      endgame_dbl_click_right = false;
    }
    if(master.get_digital_new_press(endgameBtnRight)){
      if(endgame_dbl_click_right) {
        log("%lld | RIGHT ENDGAME FIRED\n", op_control_timer.getTime());
        endgame_d_p.setState(HIGH);
      }
      else endgame_dbl_click_right = true;
      endgame_click_timer_right.reset();
    }
    // if(master.getButtonState(endgameBtnLeft) && master.getButtonState(endgameBtnRight)){
    //   endgame_d_p.setState(HIGH);
    //   endgame_s_p.setState(HIGH);
    // }

    // master.print(2, 0, "DRIVE RPM | %lf %lf\n", front_l.get_actual_velocity(), front_r.get_actual_velocity());
		// driveHandleInput();
		shooterHandleInput();
		intakeHandleInput();
		if((master.get_digital_new_press(DIGITAL_UP) || partner.get_digital_new_press(DIGITAL_UP)) && g_mag_disc_count < 3)	g_mag_disc_count++;
		if((master.get_digital_new_press(DIGITAL_DOWN) || partner.get_digital_new_press(DIGITAL_DOWN)) && g_mag_disc_count > 0)	g_mag_disc_count--; 


		if(g_mag_disc_count != g_mag_disc_count_lst){
			master.print(0,0, "disc count: %d  ", g_mag_disc_count.load());
			g_mag_disc_count_lst = g_mag_disc_count;
		}

		// if(angle_override_print.getTime() > 100){
		// 	angle_override_print.reset();
		// 	if (angleOverride) master.print(1, 0, "Override");
		// 	else master.print(1, 0, "Automatic");
		// }

    if(front_l.get_temperature() >= 50 || centre_l.get_temperature() >= 50 || back_l.get_temperature() >= 50 || front_r.get_temperature() >= 50 || centre_r.get_temperature() >= 50 || back_r.get_temperature() >= 50 || intake_m.get_temperature() >= 50 || flywheel_m.get_temperature() >= 50){
      master.rumble("-");
      // log("CONTROLLER RUMBLING FROM LINE 280 in file drive.cpp\n");

      delay(50);
      // moveDrive(0, 0);
      // intake.changeState(IntakeOffParams{});
      // drive.changeState(DriveIdleParams{});
      // shooter.changeState(ShooterIdleParams{});
      // flywheel.changeState(FlywheelOffParams{});
      // master.rumble("----------");

      // WAIT_UNTIL(false);
    } 
		Task::delay_until(&cycle_time, 10);
	}
}

void shiftTrans(bool state){
  _Task trans_task;
  trans_task.start([=](){
    log("%d Transmission started shifting into %s gear\n", millis(), state? "HIGH" : "LOW");
    trans_p.setState(state);
    drive.changeState(DriveIdleParams{});
    drive.waitToReachState(DriveIdleParams{});
    moveDrive(0, 0);
    delay(100);
    drive.changeState(DriveOpControlParams{});
    log("%d Transmission finished shifting into %s gear\n", millis(), state? "HIGH" : "LOW");
  });
}

void driveSpeedTest(){
  drive.changeState(DriveIdleParams{});
	drive.waitToReachState(DriveIdleParams{});
	Timer timer{"timer"};
	moveDrive(127, 0);
	double start = right_tracker.get_position()*1/36000.0 *(2.75*M_PI);
	double cur_y = start;

	while(cur_y < 120){
		cur_y = right_tracker.get_position()*1/36000.0 *(2.75*M_PI) - start;
		log("%lld, %lf, %lf, %lf\n", timer.getTime(), cur_y, centre_l.get_actual_velocity(), centre_r.get_actual_velocity());
		delay(50);
	}
	driveBrake();
	delay(100);
}