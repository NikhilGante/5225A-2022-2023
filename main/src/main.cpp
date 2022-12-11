#include "main.h"
#include "Libraries/gui.hpp"
#include "Libraries/task.hpp"
#include "Libraries/logging.hpp"
#include "Libraries/controller.hpp"
#include "auton.hpp"
#include "tracking.hpp"
#include "config.hpp"

const GUI* GUI::current_gui = &main_obj;


/* Nathan's Thoughts
change most #defines to constexpr
replace C macros with C++ equivalents (std::numbers::pi) - not super important
std::string instead of const char*
const after type (int const, char const *)
switch variadic functions to variadic templates
make more methods const
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

	// log_init();
	delay(500);
	// lift.runMachine();
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
	new_skills2();

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

//delete the move and copy constructors for controller button, auton...
//alert prints really big

void opcontrol(){
  DEBUG;

  DEBUG;
  WAIT_UNTIL(false);
}

/*
Takes 500ms to fill up with 15000 chars
Can print 120000 chars in 4.7s (5ms x 800 delay)
Can print 120000 chars in 8.7s (10ms x 800 delay)
Can print 120000 chars in 0.7s, but missed characters (No delay)
!all numbers are approx

Will still need a mutex in case it's inputting faster than it outputs.
Mutex isn't needed for all the writes because input and output to the queue happen at different locations.
However, when clearing, there may be an issue.

If the queue has 5 elems, 5 are written. However in that time, 2 more were inputted, so it clears to pos 7.
This loses the two characters.
*/