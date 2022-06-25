#include "main.h"
#include "Libraries/async.hpp"
#include "Libraries/geometry.hpp"
#include "Libraries/pid.hpp"
#include "Libraries/piston.hpp"
#include "Libraries/task.hpp"
#include "Libraries/timer.hpp"

#include "Libraries/logging.hpp"

#include "config.hpp"
#include "tracking.hpp"

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	_Controller::init();
	log_init();
	// update_t.start();
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
struct MoveToTargetParams{
	// replaces default constructor
	MoveToTargetParams(Position target,	bool brake,	int max_power = 127,	int exit_power = 0,	bool overshoot = false, double end_error_r = 0.5,	double end_error_a = 5.0):
		target(target), brake(brake), max_power(max_power), exit_power(exit_power), overshoot(overshoot), end_error_r(end_error_r), end_error_a(end_error_a)
		{}
  Position target;
  bool brake;
  int max_power = 127;
  int exit_power = 0;
  bool overshoot = false;
  double end_error_r = 0.5;
  double end_error_a = 5.0;
};

void moveDrive(double x, double y, double a){
	front_l.move(x + y + a);
  front_r.move(-x + y - a);
  back_l.move(-x + y + a);
  back_r.move(x + y - a);
}

void moveToTargetFn(void* params){
  MoveToTargetParams* args_ptr = (MoveToTargetParams*)_Task::get_params(params); // stores args locally
	MoveToTargetParams args = *args_ptr;

  delete args_ptr;  // frees memory
  args_ptr = nullptr; // params shouldn't point to anything

  printf("started mtt, %lf %lf %lf, %d\n", args.target.x, args.target.y, args.target.angle, args.max_power);
	// moveDrive(args.max_power, 0);
	while(true){
		delay(10);
	}
}

struct turnParams{
  double turn_power;
	string ignore;
};


void turnFn(void* params){
  turnParams* args_ptr = (turnParams*)_Task::get_params(params); // stores args locally
	turnParams args = *args_ptr;

  delete args_ptr;  // frees memory
  args_ptr = nullptr; // params shouldn't point to anything

  printf("started turnFn, %lf %s\n", args.turn_power, args.ignore.c_str());
	// moveDrive(0, args.turn_power);
	while(true){
		delay(10);
	}
}


void opcontrol() {
	double power_x, power_y, power_a;
	bool flywheel_on = false;
	Timer flywheel_print_timer{"flywheel_timer"};
	master.clear();

	pros::Rotation rotation_sensor(5);
	rotation_sensor.set_data_rate(5);
	rotation_sensor.reset_position();
	Timer print_timer{"print_timer"};
	Timer log_timer{"log_timer"};

	long rot_vel;
	int motor_speed = 80;
	master.print(2,0, "motor_speed:%d ", motor_speed);

	while(true){

    // power_x = master.get_analog(ANALOG_LEFT_X);
    // power_y = master.get_analog(ANALOG_LEFT_Y);
		// power_a = master.get_analog(ANALOG_RIGHT_X);

		// moveDrive(power_x, power_y, power_a);

		rot_vel = 3*60*rotation_sensor.get_velocity()/360;
		// rot_vel /= 36000;
		if(print_timer.get_time() >= 50){
			// log_timer.reset();
			// log("%d val: %d last: %d, mod360: %d, vel:%d\n", millis(), cur, last, (rotation_sensor.get_position()/100)%360, rotation_sensor.get_velocity());
			// log_timer.print();

			// printf("%d| rpm:%.2lf, deg/sec:%.2lf, temp| motor1:%lf, motor2:%lf current| motor1:%d, motor2:%d\n", millis(), 60*(double)(rot_vel)/36000, (double)(rot_vel)/100, flywheel_back.get_temperature(), flywheel_front.get_temperature(), flywheel_back.get_current_draw(), flywheel_front.get_current_draw());
			printf("%d| rpm:%ld, temp| motor1:%lf, motor2:%lf current| motor1:%d, motor2:%d\n", millis(), rot_vel, flywheel_back.get_temperature(), flywheel_front.get_temperature(), flywheel_back.get_current_draw(), flywheel_front.get_current_draw());

			// logfile = fopen("/usd/log.txt","a");
			// fprintf(logfile, "%d hi\n", millis());
			// fclose(logfile);
			print_timer.reset();
		}
		if(master.get_digital_new_press(DIGITAL_UP)){
			motor_speed += 5;
			if(motor_speed > 127) motor_speed = 127;
			master.print(2,0, "motor_speed:%d ", motor_speed);

		}
		if(master.get_digital_new_press(DIGITAL_DOWN)){
			motor_speed -= 5;
			if(motor_speed < 0) motor_speed = 0;
			master.print(2,0, "motor_speed:%d ", motor_speed);
		}

		if(master.get_digital_new_press(DIGITAL_A)){
			flywheel_on = !flywheel_on;
			if(flywheel_on){
				flywheel_back.move(motor_speed);
				flywheel_front.move(motor_speed);
			}
			else{
				flywheel_back.move(0);
				flywheel_front.move(0);
			}
		}
		if(flywheel_print_timer.get_time() > 150){
			// master.print(0,0, "rpm:%.2lf", 60*(double)(rot_vel)/36000);
			master.print(0,0, "rpm:%ld", -rot_vel);
			master.print(1, 0, "f:%d b:%d", (int)flywheel_front.get_temperature(), (int)flywheel_back.get_temperature());


			flywheel_print_timer.reset();
		}
		delay(10);
	}
}


/*
void opcontrol() {
	pros::Rotation rotation_sensor(5);
	rotation_sensor.set_data_rate(5);
	rotation_sensor.reset_position();
	int cur, last = 0;
	int count = 1;
	Timer print_timer{"print_timer"};
	Timer log_timer{"log_timer"};
	while(true){
		cur = rotation_sensor.get_position();
		if(print_timer.get_time() > 20){
			log_timer.reset();
			log("%d val: %d last: %d, mod360: %d, vel:%d\n", millis(), cur, last, (rotation_sensor.get_position()/100)%360, rotation_sensor.get_velocity());
			log_timer.print();
			printf("%d val: %d last: %d, mod360: %d, vel:%d\n", millis(), cur, last, (rotation_sensor.get_position()/100)%360, rotation_sensor.get_velocity());
			// logfile = fopen("/usd/log.txt","a");
			// fprintf(logfile, "%d hi\n", millis());
			// fclose(logfile);
			print_timer.reset();
		}
		if(cur != last){
			printf("**********HEYcur: cur %d last %d diff %d count %d\n", cur, last, cur-last, count);
			count = 1;
		}
		else count++;
		last = cur;
		delay(1);
	}
}
*/
