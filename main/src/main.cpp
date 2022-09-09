#include "main.h"
#include "Libraries/geometry.hpp"
#include "Libraries/pid.hpp"
#include "Libraries/piston.hpp"
#include "Libraries/task.hpp"
#include "Libraries/timer.hpp"
#include <atomic>

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
	lcd::initialize();
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

	pros::Rotation rotation_sensor(5, true);	// configures rotation sensor in port 5
	rotation_sensor.set_data_rate(5);	// gets data from rotation sensor every 5ms
	rotation_sensor.reset_position();
	Timer print_timer{"print_timer"};
	Timer log_timer{"log_timer"};

	long rot_vel;
	int vel_target = 2350;
	master.print(2,0, "vel_target:%d ", vel_target);

	// PID stuff
	// Kp, Ki, kD, and bias
	// PID flywheel_PID(1.0, 0.0, 0.0, 0.0);
	Timer pid_timer{"pid_timer"};
	atomic<double> error, last_error;
	double kB = 0.0385;	// this approximately converts a target velocity into a motor voltage
	double kP = 0.35, Ki = 0.0, kD = 0.0;
	double proportional, integral, derivative;
	double output; // what power goes to the motors
	// pwm to vel
	Task flywheel_t([&](){
		int shot_counter = 0;
		while(true){
			if(master.get_digital_new_press(DIGITAL_A))	shot_counter = 0;
			if(fabs(error.load()) < 30 && shot_counter < 3){
					printf("STARTED SHOOTING\n");
					indexer_p.set_state(HIGH);	
					delay(75); // wait for indexer to extend
					printf("FINISHED SHOT\n");
					indexer_p.set_state(LOW);
					delay(100);// wait for indexer to retract
					printf("FINISHED Retraction\n");
					shot_counter++;
			}
			delay(10);
		}
	});
	while(true){
		rot_vel = 3*60*rotation_sensor.get_velocity()/360;	// actual velocity of flywheel
		error = vel_target - rot_vel;
		proportional = kP * error;
		if(output < 127){	// don't integrate if power exceeds max
			integral += Ki * error * pid_timer.get_time();
		}
		derivative = kD * (error - last_error) / pid_timer.get_time();
		last_error = error.load();
		pid_timer.reset();
		output = vel_target * kB + proportional + integral + derivative;
		// for graphing purposes
		printf("%d, %d, %d, %ld, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf\n", millis(), disc_sensor.get_value(), vel_target, rot_vel, error.load(), output, vel_target * kB, proportional, integral, derivative);
		output = std::clamp(output, -1.0, 127.0);	// decelerates at -1.0 at the most
		flywheel_m.move(output);

		// Uncomment to print flywheel data
		/*
		if(print_timer.get_time() >= 50){

			printf("%d| rpm:%ld, temp:%lf current:%d\n", millis(), rot_vel, flywheel_m.get_temperature(), flywheel_m.get_current_draw());
			print_timer.reset();
		}
		*/
		// 120 -150ms between telling piston to fire before sensor detects disc
		if(master.get_digital_new_press(DIGITAL_X)){
			printf("TARGET CHANGED");
			vel_target = 1680;
			master.print(2,0, "vel_target:%d ", vel_target);
		} 
		if(master.get_digital_new_press(DIGITAL_Y)){
			vel_target = 2350;
			master.print(2,0, "vel_target:%d ", vel_target);
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

		if(flywheel_print_timer.get_time() > 50){
			master.print(0,0, "rpm:%ld temp:%d", rot_vel, (int)flywheel_m.get_temperature());
			flywheel_print_timer.reset();
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
