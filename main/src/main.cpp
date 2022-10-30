#include "main.h"
#include "Libraries/geometry.hpp"
#include "Libraries/pid.hpp"
#include "Libraries/piston.hpp"
#include "Libraries/timer.hpp"
#include "Libraries/state.hpp"
#include "Libraries/gui.hpp"
#include "Libraries/task.hpp"

#include "Libraries/logging.hpp"

#include "lift.hpp"
#include "pros/misc.h"
#include "tracking.hpp"
#include "drive.hpp"
#include "config.hpp"

#include "pros/llemu.hpp"
#include "pros/rtos.h"

const GUI* GUI::current_gui = &main_obj;


/* Nathan's Thoughts
why is queuePrintFile not a member function of Queue?
change most #defines to constexpr
replace C macros with C++ equivalents (M_PI) - not super important
std::string instead of const char*
const after type (int const, char const *)
acivate using namespace pros in main.h
try to remove using namespace std if everyone is ok with that
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
	// log_init();
	// lcd::initialize();
	tracking.g_pos = {31.0, 11.5, 0.0};
	// tracking.g_pos = {70.0, 129.5, M_PI};
	_Task_ tracking_task("tracking_update_task");
	tracking_task.start(trackingUpdate);
  GUI::init();
	Data::init();
	// _Controller::init();
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

Data data1;

//make page 7 active. figure out repeated deprecate on page 5
//make alert queue if already flashing
//delete the move and copy constructors for controller button, auton...
//alert prints really big




//push in entirely, and then use queue.print
void opcontrol() {
  for(int i = 0; i < 100; i++){
		for(int j = 0; j < 25; j++){	
			data1.print("%05d ", 25*i+j);

      // if(Data::queue.getDataSize() > 50) queuePrintFile(Data::queue, Data::log_file, "/usd/log.txt");
		}
		data1.print("\\n\n");
    delay(50);
	}
  // data1.print("abcdefghijklmnopqrstuvwxyz");
  // queuePrintFile(Data::queue, Data::log_file, "/usd/log.txt");
  
  // printf("Is empty: %d\n", Data::queue.isEmpty());
  // Data::queue.pop();
  // printf("Is empty: %d\n", Data::queue.isEmpty());

  // queuePrintFile(Data::queue, Data::log_file, "/usd/log.txt");
  WAIT_UNTIL(false);
}