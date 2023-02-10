#include "main.h"
#include "Libraries/gui.hpp"
#include "Libraries/task.hpp"
#include "Libraries/logging.hpp"
#include "Libraries/controller.hpp"
#include "auton.hpp"
#include "menu.hpp"
#include "tracking.hpp"

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	Logging::init();
  GUI::init();
	_Controller::init();

	tracking.g_pos = {30.75, 9.0, degToRad(0.0)};	// ACTUAL SKILLS
	// tracking.g_pos = {70.0, 129.5, std::numbers::pi};
	_Task tracking_task("Tracking Update");
	tracking_task.start(trackingUpdate);

	delay(500);
			if(pros::c::battery_get_voltage() < 12200){
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
void autonomous() {
	Auton::runAuton();
}

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

void auton2func(){
	printf("whatup\n");
}

void auton3func(){
	printf("ayyyy\n");
}

Auton auton1("autonStack", autonStack);
Auton auton2("autonAWP", autonAWP);
Auton auton3("autonLine", autonLine, E_Auton_Reset_Types::far);
Auton auton4("Skills", fullSkills);

//print logs as .rtf
//use \033[1m for bold
//use abbreviated template
void opcontrol() {
  DEBUG;
  
  DEBUG;
  WAIT_UNTIL(false);
}
