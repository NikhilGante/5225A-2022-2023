#include "main.h"
#include "Libraries/gui.hpp"
#include "Libraries/task.hpp"
#include "Libraries/logging.hpp"
#include "Libraries/controller.hpp"
#include "Subsystems/flywheel.hpp"
#include "auton.hpp"
#include "tracking.hpp"
#include "config.hpp"
#include "Subsystems/intake.hpp"
#include "util.hpp"


const GUI* GUI::current_gui = &main_obj;


/* To make it better
const after type (int const, char const *)
make more methods const
alert prints really big
*/


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

	tracking.g_pos = {31.0, 11.5, 0.0};
	// tracking.g_pos = {70.0, 129.5, std::numbers::pi};
	_Task tracking_task("tracking_update_task");
	tracking_task.start(trackingUpdate);

	delay(500);
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
	newSkills2();
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

void opcontrol(){
  DEBUG;

  DEBUG;
  WAIT_UNTIL(false);
}