#include "../include/main.h"
#include <cmath>
#include "../include/api.h"
#include "tracking.hpp"
#include "config.hpp"
#include "auton.hpp"

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
void testing(){

}


pros::Distance distance_sensor1 (4); // Left Distance sensor Sensor
auton auton1("AutonName1",USE_DISTANCE_SENSOR, 0, 90, testing, distance_sensor1, false);
auton auton2("AutonName2",0, USE_DISTANCE_SENSOR, 90, testing, distance_sensor1);
auton auton3("AutonName3",0, 0, 90, testing, distance_sensor1);


void opcontrol() {
	delay(500);
	gA = 0.0*M_PI/180;
	gX = 0.0;
	gY = 0.0;
	// Controller master (E_CONTROLLER_MASTER);
	master.clear();

	Task my_task(tracking);



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
		
	// 	screen::print(TEXT_MEDIUM, 3, "L: %d  , R: %d  ", LeftEncoder.get_value(), RightEncoder.get_value());
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
	
	xDriveMTT(24, 24, 90, E_Brake_Modes::brake, 0.5);
	// xDriveTTA(180, E_Brake_Modes::brake, 1);


	// while (1){
	// 	printf("Left Encoder(Ticks): %d  | Right Encoder(Ticks): %d  | Back Encoder(Ticks): %d  \n",LeftEncoder.get_value(), RightEncoder.get_value(), BackEncoder.get_value());
	// 	printf("Left Encoder(Inches): %lf  | Right Encoder(Inches): %lf  | Back Encoder(Inches): %lf  \n",LeftEncoder.get_value()*TICKS_TO_INCHES, RightEncoder.get_value()*TICKS_TO_INCHES, BackEncoder.get_value()*TICKS_TO_INCHES);
	// 	delay(100);
	// }


}