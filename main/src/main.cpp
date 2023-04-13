#include "main.h"
#include "Libraries/geometry.hpp"
#include "Libraries/pid.hpp"
#include "Libraries/piston.hpp"
#include "Libraries/timer.hpp"
#include "Libraries/state.hpp"

#include "Libraries/task.hpp"
#include "Libraries/logging.hpp"

#include "auton.hpp"
#include "menu.hpp"

#include "programutil.hpp"
#include "pros/misc.h"
#include "tracking.hpp"
#include "drive.hpp"
#include "config.hpp"
#include "Libraries/controller.hpp"

#include "pros/llemu.hpp"
#include "pros/rtos.h"

// Subsystem includes
#include "Subsystems/shooter.hpp"
#include "Subsystems/flywheel.hpp"
#include "Subsystems/intake.hpp"
#include "util.hpp"

#include <cmath>


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */


void initialize() {
	lcd::initialize();

	tracking.g_pos = {0.0, 0.0, 0.0};

	log_init();

	delay(300);
	_Task tracking_task("tracking_update_task");
	tracking_task.start(trackingUpdate);
	_Controller::init();


	drive.runMachine();
	intake.runMachine();
	flywheel.runMachine();
	shooter.runMachine();

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

void competition_initialize() {
	program_selector();
}

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

	WAIT_UNTIL(!gyro.is_calibrating());

	Auton::runAuton();

}


Auton auton1("autonStack", autonStack);
Auton auton2("autonAWP", autonAWP2);
Auton auton3("autonLine", autonLine, E_Auton_Reset_Types::far);
Auton auton4("Blank", [](){master.print(0, 0, "BLANK AUTON");});



void opcontrol() {
	// while(true){
	// 	printf("Sensors: %d %d %d %d\n", intk_lim_switch.get_value(), intk_dist.get(), intk_ds.get_value(), uptk_ds.get_value());
	// 	// printf("%d\n", diskInIntake());
	// 	delay(10);
	// }

	util_selector();
	// driverPractice();
}