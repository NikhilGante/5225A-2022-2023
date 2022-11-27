#include "main.h"
#include "Libraries/geometry.hpp"
#include "Libraries/pid.hpp"
#include "Libraries/piston.hpp"
#include "Libraries/timer.hpp"
#include "Libraries/state.hpp"

#include "Libraries/task.hpp"

#include "Libraries/logging.hpp"
#include "auton.hpp"

#include "lift.hpp"
#include "pros/misc.h"
#include "pros/misc.hpp"
#include "pros/motors.h"
#include "pros/screen.h"
#include "tracking.hpp"
#include "drive.hpp"
#include "config.hpp"
#include "Libraries/controller.hpp"

#include "pros/llemu.hpp"
#include "pros/rtos.h"
#include <iostream>
#include <utility>

using namespace pros;
void initialize() {
	// log_init();
	//lcd::initialize();
	// tracking.g_pos = {34.75, 11.25, degToRad(0.0)};	// newSkills1
	//tracking.g_pos = {108.0, 129.75, degToRad(180.0)};	// new_skills2
	// tracking.g_pos = {11.25, 23.5, degToRad(90.0)};	// new_skills3
	// tracking.g_pos = {129.75, 116.5, degToRad(-90.0)}; // new_skills4


	// tracking.g_pos = {35.25, 11.25, 0.0};	// skills1
	// tracking.g_pos = {68.00, 129.25, M_PI};	// skills2
	// tracking.g_pos = {72.0, 11.25, 0.0};	// skills3

	// tracking.g_pos = {0.0, 0.0, 0.0};

	// log_init();
	//_Task tracking_task("tracking_update_task");
	//tracking_task.start(trackingUpdate);
	// Data::init();
	//_Controller::init();
	//delay(300);
	// lift.runMachine();
	//drive.runMachine();

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

void autonomous() {
}
int sgn(int n){ //sign function
	if (n>0) return 1;
	else if (n<0) return -1;
	else return 0;
}
/*
void turnToAngle(double target){
	double cur_angle, p, i, d, kp, ki, kd, error, last_error, output, upper_limit, lower_limit;
	uint32_t time, old_time;
	old_time = millis();
	kp = 2 ;
	ki = 17;
	kd = 10;
	upper_limit = 20;
	lower_limit = 5;
	
	do{
		//delay(10); //delay at the start so that we are not dividing by 0
		time = millis() - old_time; //this works :D
		old_time = millis();
		cur_angle = tracking.g_pos.a; //Not sure if this works...
		error = target - cur_angle; //error = difference
		p = error*kp; //get p

		if (sgn(error) != sgn(last_error) || fabs(error) > upper_limit || fabs(error) < lower_limit) i = 0; //if overshot, then reset integration.
		else i += error*time*ki; //normal i

		d = (error-last_error)/time - kd; //get d

		output = p+i+d; //PID
		last_error = error;

		screen::print(pros::E_TEXT_SMALL, 0, "p = %.4f, i = %.4f, d = %.4f", p,i,d);
		screen::print(pros::E_TEXT_SMALL, 1, "o = %.4f, cA = %.4f, e = %.4f", output,cur_angle, error);
		printf("o = %.4f, cA = %.4f, e = %.4f", output,cur_angle, error);
		delay(10);
	} while (fabs(error) > 0.5); //do while loop is for calculating error before we compare it.
}
*/

void opcontrol() {
	// _Task trackingUpdate;
	// turnToAngle(45);
	// cout << "ENDING" << endl; //Lets us know that its ending.

	int cur_speed, target = 20, diff, cycle_count = 0, expected_cycle; 
	bool jammed = false;
	Motor motor(12);
	Controller master(E_CONTROLLER_MASTER);

	master.clear();

	motor.move(target);

	while(jammed != true){
		screen::print(pros::E_TEXT_SMALL, 1, "Target:%d  Cur:%d  cycles: %d", target, cur_speed, cycle_count);
		cur_speed = motor.get_actual_velocity();
	
		// master.print(0,0,"%d", cur_speed);
		// printf("%d\n", cur_speed); 
		cout << cur_speed << endl;

		if (abs(target) > 110) { 
			diff = 5;
			expected_cycle = 10;
		} else if(abs(target) > 50 ) {
			diff = 3;
			expected_cycle = 6;
		} else if(abs(target) > 10) {
			diff = 1;
			expected_cycle = 3;
		} else diff = 0;

		if (cur_speed < abs(target) - diff){
			cycle_count += 1;
		} else {
			cycle_count -= 1;
		}

		if (cycle_count == expected_cycle){
			jammed = true;
		}

		delay(10);
		// master.clear_line(0);
	}

}

/*
TO DO:
Check for encoders, motors, 
Tracking is updating in the background
*/