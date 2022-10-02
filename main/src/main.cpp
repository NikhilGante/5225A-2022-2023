#include "main.h"
#include "Libraries/geometry.hpp"
#include "Libraries/pid.hpp"
#include "Libraries/piston.hpp"
#include "Libraries/timer.hpp"
#include "Libraries/state.hpp"

#include "Libraries/task.hpp"

#include "Libraries/logging.hpp"
#include "auton.hpp"

#include "lift.hpp"
#include "pros/misc.h"
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
	// log_init();
	lcd::initialize();
	tracking.g_pos = {34.75, 11.25, degToRad(0.0)};	// newSkills1
	// tracking.g_pos = {108.0, 129.75, degToRad(180.0)};	// new_skills2
	// tracking.g_pos = {11.25, 23.5, degToRad(90.0)};	// new_skills3
	// tracking.g_pos = {129.75, 116.5, degToRad(-90.0)}; // new_skills4


	// tracking.g_pos = {35.25, 11.25, 0.0};	// skills1
	// tracking.g_pos = {68.00, 129.25, M_PI};	// skills2
	// tracking.g_pos = {72.0, 11.25, 0.0};	// skills3

	// tracking.g_pos = {0.0, 0.0, 0.0};

	log_init();
	_Task_ tracking_task("tracking_update_task");
	tracking_task.start(trackingUpdate);
	// Data::init();
	// _Controller::init();
	delay(500);
	// lift.runMachine();
	drive.runMachine();

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
// start (70, 129.5)
// end: (71, 11.5)

// coords of high goal (approx)
// Red: (18.0, 123.0)

// STACK TRACE
// 0x39083ec
// 0x39083e0
// 0x7800440
// 0x780150c
// 0x7801118
// 0x384cd54
// 0x384b410
// 0x38458a0


void opcontrol() {
	// flattenAgainstWallSync();	
	// tracking.reset();
	// turnToTargetSync({40.0, -40.0}, true);
	// WAIT_UNTIL(false);
	new_skills1();
	// new_skills2();
	// new_skills3();
	// new_skills4();
	// skills2();
	WAIT_UNTIL(false);
	// front_r.move_relative(0, 200);
  // centre_r.move_relative(0, 200);
  // back_r.move_relative(0, 200);
	while(true){
		handleInput();
		delay(10);
	}
	// turnToTargetAsync({-25.0, 40.0});
	// tracking.waitForComplete();
	// drive.changeState(DriveMttParams{{5.0, 30.0}});
	// tracking.waitForComplete();
	// drive.changeState(DriveTurnToAngleParams{50.0});
	// drive.changeState(DriveTurnToTargetParams{{10.0, 5.0}});
	// turnToTargetSync({10.0, 10.0});

	// auton50pts();
	// skills1();
	// skills2();
	// skills3();



	// lcd::print(7, "total: %d", total.get_time());
	// log("total: %d", total.get_time());


	WAIT_UNTIL(false);

	Data data;
	for(int i = 0; i < 100; i++){
		master.print(0,0, "h%d", i);
		master.rumble("-");

		delay(30);
	}

	// char txt[] = "wassup beee\n";
	// data.print(txt);
	// char txt2[] = "hello uwu\n";
	// data.print(txt2);
	// data.print("hello\n");
	data.print("my name is ");
	// data.print("bob");
	// data.print("nice to\t eat you\n");
	// data.print("kbye");
	for(int i = 0; i < 100; i++){
		for(int j = 0; j < 5; j++){	
			log("aaaaaaaaaa");
		}
		// data.print("\n");
		log("\n");
		// delay(1);
	}
	log("omg I agre%de %lf\n", 5, 42.5);
	// delay(5000);
	lcd::print(0,"closed");
	WAIT_UNTIL(false);

	// master.clear();
	// uint32_t i3 = 0;
	// string str = "ayoo";
	// master.print(0,0, str);
	// delay(1000);
	// master.clear_line(0);

	// while(true){
	// 	master.print(0,0, str);
	// 	master.clear_line(0);
	// 	master.rumble("-");
	// 	delay(50);
	// }
	// _Task_ tarsk{"tarsk"};
	// tarsk.start([](){
	// 	while(true){
	// 		lcd::print(0, "position: %lf", b_lift_m.get_position());
	// 		lcd::print(6, "index: %d", lift_index);

	// 		if(master.get_digital_new_press(DIGITAL_A))	lift.changeState(LiftMTTParams{lift_arr[0]});
	// 		if(master.get_digital_new_press(DIGITAL_B))	lift.changeState(LiftResetParams{});
	// 		if(master.get_digital_new_press(DIGITAL_Y))	lift.changeState(LiftIdleParams{});

	// 		if(master.get_digital_new_press(DIGITAL_UP))	lift.changeState(LiftMTTParams{lift_arr[++lift_index]});
	// 		if(master.get_digital_new_press(DIGITAL_DOWN))	lift.changeState(LiftMTTParams{lift_arr[--lift_index]});
			
	// 		delay(10);
	// 	}
	// });
	// lcd::print(5, "hello");
	// delay(1000);
	// lift.waitToReachState(LiftResetParams{});
	// lcd::print(5, "Reset");
	// WAIT_UNTIL(false);


	// moveToTarget({0.0, 40.0, 180.0}, brake_modes::brake, 50.0);
	// moveToTarget({20.0, 30.0, -40.0});
	// moveToTarget({-20.0, 20.0, 50.0});

	// moveToTarget({0.0, 0.0, 0.0});

	WAIT_UNTIL(false);

	lcd::print(6, "DONE");
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
