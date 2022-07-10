#include "main.h"
#include "config.hpp"
#include "drive.hpp"
#include "pros/llemu.hpp"

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	lcd::initialize();
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



void opcontrol() {
	bool intk_on = false;
	master.clear();
	delay(50);
	master.print(0,0, "curvature: %lf    ", angle_curvature);
	delay(50);
	master.print(1,0, "press up or down to ");
	delay(50);
	master.print(2,0, "change curvature");
	lcd::print(0, "press a to turn intake on & off");
	lcd::print(1, "intake should be in port 4");

	uint32_t screen_timer = millis();
	while(true){

		handleInput();
		if(master.get_digital_new_press(DIGITAL_UP) && angle_curvature < 10.0)	angle_curvature += 0.2;
		if(master.get_digital_new_press(DIGITAL_DOWN) && angle_curvature > 0.0)	angle_curvature -= 0.2;
		
		
		if(master.get_digital_new_press(DIGITAL_A)){
			intk_on = !intk_on;
			if(intk_on)	intk_m.move(127);
			else intk_m.move(0);
		}

		if(millis() - screen_timer > 50){
			master.print(0,0, "curvature: %lf    ", angle_curvature);
		}

		delay(10);
	}

}
