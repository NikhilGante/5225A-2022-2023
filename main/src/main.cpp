#include "main.h"
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

void moveDrive(double x, double y, double a){
	front_l.move(x + y + a);
  front_r.move(-x + y - a);
  back_l.move(-x + y + a);
  back_r.move(x + y - a);
}



void opcontrol() {
	bool flywheel_on = false;
	Timer flywheel_print_timer{"flywheel_timer"};
	master.clear();

	pros::Rotation rotation_sensor(5);	// configures rotation sensor in port 5
	rotation_sensor.set_data_rate(5);	// gets data from rotation sensor every 5ms
	rotation_sensor.reset_position();
	Timer print_timer{"print_timer"};
	Timer log_timer{"log_timer"};

	long rot_vel;
	int vel_target = 1800;
	master.print(2,0, "vel_target:%d ", vel_target);

	// PID stuff
	// Kp, Ki, kD, and bias
	PID flywheel_PID(1.0, 0.0, 0.0, 0.0);
	Timer pid_timer{"pid_timer"};
	double error, last_error;
	double kB = 5.0;	// this approximately converts a target velocity into a motor voltage
	double kP = 0.0, Ki = 0.0, kD = 0.0;
	double proportional, integral, derivative;
	double output; // what power goes to the motors
	while(true){
		rot_vel = 3*60*rotation_sensor.get_velocity()/360;	// actual velocity of flywheel
		error = vel_target - rot_vel;
		proportional = kP * error;
		if(output < 127){	// don't integrate if power exceeds max
			integral += Ki * error * pid_timer.get_time();
		}
		derivative = (error - last_error) / pid_timer.get_time();
		pid_timer.reset();
		output = vel_target * kB + proportional + integral + derivative;
		flywheel_back.move(output);
		flywheel_front.move(output);


		if(print_timer.get_time() >= 50){

			printf("%d| rpm:%ld, temp| motor1:%lf, motor2:%lf current| motor1:%d, motor2:%d\n", millis(), rot_vel, flywheel_back.get_temperature(), flywheel_front.get_temperature(), flywheel_back.get_current_draw(), flywheel_front.get_current_draw());
			print_timer.reset();
		}
		if(master.get_digital_new_press(DIGITAL_UP)){	// Increment the flywheel speed
			vel_target += 20;
			if(vel_target > 3000) vel_target = 3000;
			master.print(2,0, "vel_target:%d ", vel_target);
		}
		if(master.get_digital_new_press(DIGITAL_DOWN)){	// Decrement the flywheel speed 
			vel_target -= 20;
			if(vel_target < 0) vel_target = 0;
			master.print(2,0, "vel_target:%d ", vel_target);
		}

		// if(master.get_digital_new_press(DIGITAL_A)){	// Toggle the flywheel on/off when A is pressed
		// 	flywheel_on = !flywheel_on;
		// 	if(flywheel_on){
		// 		flywheel_back.move(vel_target);
		// 		flywheel_front.move(vel_target);
		// 	}
		// 	else{
		// 		flywheel_back.move(0);
		// 		flywheel_front.move(0);
		// 	}
		// }
		// Print flywheel data to the controller screen
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
