#include "../include/main.h"
#include <cmath>
#include "../include/api.h"
#include "tracking.hpp"
#include "config.hpp"
#include "auton.hpp"
#include "PID.hpp"

using namespace pros;
using namespace std;


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {}

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
void test(){
	screen::print(pros::E_TEXT_LARGE, 1, "Running auton Test");
}







pros::Distance distance_sensor1 (4); //  Distance Sensor

// Autons

auton a1("RightShoot",FIELD_MAX_MM-7.5, USE_DISTANCE_SENSOR, -90, autonM2, distance_sensor1, false);
auton a2("LeftShoot",USE_DISTANCE_SENSOR, 7.5, 0, autonM, distance_sensor1);
auton a3("LeftRollers",USE_DISTANCE_SENSOR, 7.5, 0, auton3, distance_sensor1);



void opcontrol() {

	gA = 0;
	gX = 0;
	gY = 0;
	

	Task my_task(tracking);

/*
	PID robot(11, 0, 2500, 24, 2);

	double x = 24.0, y = 24.0, a = -90;
	// double x = 10.0, y = -10.0, a = 0;
	double t, b;
	double PID_H;
	double mX, mY, mA;
	double scale_factor;
	double min_power = 25, max_power = 90;

	double h;
	double xOffSet, yOffSet, aOffset;
	do{
		xOffSet = x-gX;
		yOffSet = y-gY;
		aOffset = a-(gA*180/M_PI);

		h = sqrt(xOffSet*xOffSet+yOffSet*yOffSet);
		b = atan2(xOffSet, yOffSet)*180/M_PI, t = (b)+(0-(gA*180/M_PI));

		PID_H = robot.calculate(h);

		mX = sin(t*M_PI/180)*PID_H, mY = cos(t*M_PI/180)*PID_H, mA = aOffset*4;


		if (fabs(mX)+fabs(mY)+fabs(mA) > max_power){
			scale_factor = (fabs(mX)+fabs(mY)+fabs(mA))/max_power;
		} else if (fabs(mX)+fabs(mY)+fabs(mA) < min_power){
			scale_factor = (fabs(mX)+fabs(mY)+fabs(mA))/min_power;
		} else scale_factor = 1;

		mX /= scale_factor;
		mY /= scale_factor;
		mA /= scale_factor;




		moveXDrive(mX, mY, mA);
		cout << gX << ' ' << gY << endl;
		cout << mX << ' ' << mY << ' ' << mA << endl;
		cout << h << ' ' << PID_H << endl << endl;


		delay(10);

	} while (h>0.5 || fabs(aOffset)>1);
	front_l.move_relative(0, 200);
	front_r.move_relative(0, 200);
	back_l.move_relative(0, 200);
	back_r.move_relative(0, 200);

*/


	//xDriveARC(0, 48, 24, 24, 10, E_Brake_Modes::brake, 2);
	//screen::print(pros::E_TEXT_LARGE, 1, "ENDED");



	// autonM2();
	
	// xDriveARC(0, 48, 24, 24, 10, E_Brake_Modes::brake, 2);
	// xDriveMTT(48, 48, 90, E_Brake_Modes::brake, 0.5, 90, 40, true, 60);


	// auton::program1();
	// auton::program2();
	

	// while (!master.get_digital_new_press(E_CONTROLLER_DIGITAL_RIGHT)){
	// 	if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_RIGHT)){
	// 		distance_lr += 0.01;
	// 	} else if(master.get_digital_new_press(E_CONTROLLER_DIGITAL_LEFT)){
	// 		distance_lr -= 0.01;
	// 	} else if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_X)){
	// 		LeftEncoder.reset();
	// 		RightEncoder.reset();
	// 		delay(5);
	// 		LeftEncoder.reset();
	// 		RightEncoder.reset();
	// 		gA = 0.0;
	// 		gX = 0.0;
	// 		gY = 0.0;
	// 	}
		
	// 	screen::print(TEXT_MEDIUM, 3, "L: %d  , R: %d  , B: %d                     ", LeftEncoder.get_value(), RightEncoder.get_value(), BackEncoder.get_value());
	// 	screen::print(TEXT_MEDIUM, 4, "X: %lf  , Y: %lf   , A: %lf      , LR: %lf  \n", gX, gY, gA*(180/M_PI), distance_lr);

	// 	// printf("X: %lf  , Y: %lf   , A: %lf      , LR: %lf  \n", gX, gY, gA*(180/M_PI), distance_lr);
	// 	// printf("L: %d  , R:  %d \n", LeftEncoder.get_value(), RightEncoder.get_value());

	// 	master.print(0, 0, "L: %d   , R: %d      ", LeftEncoder.get_value(), RightEncoder.get_value());
	// 	delay(50);
	// 	master.print(1, 0, "X:%.1lf,Y: %.1lf,A:%.4lf\n", gX, gY, gA*(180/M_PI));
	// 	delay(50);
	// 	master.print(2, 0, "%lf",distance_lr);
	// 	delay(50);
	// }
	
	
}