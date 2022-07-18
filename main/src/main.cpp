#include "main.h"
#include "Libraries/geometry.hpp"
#include "Libraries/pid.hpp"
#include "Libraries/piston.hpp"
#include "Libraries/task.hpp"
#include "Libraries/timer.hpp"
#include "Libraries/state.hpp"

#include "Libraries/task2.hpp"

#include "Libraries/logging.hpp"

#include "lift.hpp"
#include "tracking.hpp"
#include "drive.hpp"
#include "config.hpp"

#include "pros/llemu.hpp"
#include "pros/rtos.h"


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	log_init();
	// _Controller::init();
	lcd::initialize();
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


// Angler angler;

void my_task_fn(void* param) {
  std::cout << "Hello" << std::endl;
  // ...
	WAIT_UNTIL(false);
}
int i = -1;
int i2 = -1;

	_Task_ lcd_task("lcd_task");


void lcd_task_fn (void* params){
	try{
		i++;
		int cpy = i;
		int counter = *(int*)(params);
		while(true){
			lcd::print(cpy,"counter:%d", counter);
			// master.print(i, 0, "counter:%d", counter);
			counter++;
			// if(master.get_digital_new_press(DIGITAL_A))	lcd_task.kill();
			// if(master.get_digital_new_press(DIGITAL_A))	lcd_task.start(lcd_task_fn);

			_Task_::delay(100);
			// delay(100);
		}
	}
	catch(const TaskEndException& exception){
		// master.print(increment_controller_line(), 0, "killed");
	}

};

void screen_task_fn (void* ignore){
	try{
		i2++;
		int cpy = i2;
		
		int counter = 0;
		while(true){
			// lcd::print(i,"counter:%d", counter);
			master.print(cpy, 0, "counter:%d", counter);
			counter++;
			// if(master.get_digital_new_press(DIGITAL_Y))	lcd_task.kill();
			// if(master.get_digital_new_press(DIGITAL_A))	lcd_task.start(lcd_task_fn);

			_Task_::delay(100);
			// delay(100);
		}
	}
	catch(const TaskEndException& exception){
		// master.print(increment_controller_line(), 0, "killed");
	}

};

void opcontrol() {
	_Task_ tracking_t("tracking_task");
	tracking_t.start(tracking_update);
	// moveToTarget({0.0, 40.0, 0.0});

	moveToTarget({0.0, 40.0, 180.0}, brake_modes::brake, 50.0);
	moveToTarget({20.0, 30.0, -40.0});
	moveToTarget({-20.0, 20.0, 50.0});

	moveToTarget({0.0, 0.0, 0.0});

	WAIT_UNTIL(false);

	lcd::print(6, "DONE");
	WAIT_UNTIL(false);

	lift.run_machine();
	lift.change_state(LiftMTTParams{lift_arr[0]});
	WAIT_UNTIL(false);
	_Task_ screen_task("lcd_task");
	int start_num = 500;

	// lcd_task.start(lcd_task_fn, &start_num);

	master.clear();
	delay(50);
	log("op_control: %d\n", task_get_current());


	while(true){
		// lcd::print(0, "%d %d", arse_handle, task_get_state(arse_handle));
		// if(arse_handle)	lcd::print(0, "hi %d", arse_handle);
		// lcd::print(1, "hi %d", arse_handle);
		if(master.get_digital_new_press(DIGITAL_A))	lcd_task.start([=](){
				try{
					i++;
					int cpy = i;
					int counter = start_num;
					while(true){
						lcd::print(cpy,"counter:%d", counter);
						// master.print(i, 0, "counter:%d", counter);
						counter++;
						// if(master.get_digital_new_press(DIGITAL_A))	lcd_task.kill();
						// if(master.get_digital_new_press(DIGITAL_A))	lcd_task.start(lcd_task_fn);

						_Task_::delay(100);
						// delay(100);
					}
				}
				catch(const TaskEndException& exception){
					// master.print(increment_controller_line(), 0, "killed");
				}
			
		}	, &start_num);
		if(master.get_digital_new_press(DIGITAL_Y))	lcd_task.kill();

		if(master.get_digital_new_press(DIGITAL_B)){
			lcd_task.suspend();
			// task_suspend(tarsk);
		}
		if(master.get_digital_new_press(DIGITAL_X)){
			lcd_task.resume();
			// task_resume(tarsk);
		}
		// delay(10);
		_Task_::delay(10);
	}


}
