#include "main.h"
#include "Libraries/geometry.hpp"
#include "Libraries/pid.hpp"
#include "Libraries/piston.hpp"
#include "Libraries/task.hpp"
#include "Libraries/timer.hpp"
#include "Libraries/state.hpp"

#include "lift.hpp"

#include "config.hpp"


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	printf("yo\n");
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

// Position target;
// bool brake;
// int max_power = 127;
// int exit_power = 0;
// bool overshoot = false;
// double end_error_r = 0.5;
// double end_error_a = 5.0;

void moveDrive(double y, double a){
  front_l.move(y+a);
  front_r.move(y-a);
  back_l.move(y+a);
  back_r.move(y-a);
  center_l.move(y+a);
  center_r.move(y-a);
}


// Angler angler;

Subsystem<LiftMttParams> lift("Lift", LiftMttParams{});


void my_task_fn(void* param) {
  std::cout << "Hello" << std::endl;
  // ...
	wait_until(false);
}

void opcontrol() {
	// lift.run_machine();
	// lift.change_state(LiftMttParams{});
	task_t temp;
	cout << temp << "done" << endl;
	// cout << "before " << pros::c::task_get_state(temp) << endl;
	// printf("%d before creation:%d\n", pros::c::task_get_state(temp), E_TASK_STATE_DELETED);
	temp = pros::c::task_create(my_task_fn, (void*)"PROS", TASK_PRIORITY_DEFAULT,
                               TASK_STACK_DEPTH_DEFAULT, "My Task");
	// if(pros::c::task_get_state(temp) != E_TASK_STATE_DELETED)	printf("hi\n");
	cout << "here "<< temp << "done2" << endl;
	while (true) {
		printf("%d deleted:%d\n", pros::c::task_get_state(temp), E_TASK_STATE_DELETED);
		delay(1000);
	}
}
