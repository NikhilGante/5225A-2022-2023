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
	// gyro.reset(true);
	delay(300);
	_Task tracking_task("tracking_update_task");
	tracking_task.start(trackingUpdate);

	Data::init();
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
void disabled() {

	// master.clear();
	// master.print(0, 2, "hii");
}

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
  master.clear();

  

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

	// spinRoller();

	autonLine();






  // setFlywheelVel(2260);
//   setFlywheelVel(barrier_rpm);
//   shoot(3, false, true);

//   WAIT_UNTIL(false);
  
  // setFlywheelVel(2260);
  // setFlywheelVel(barrier_rpm);
  // shoot(3, false, true);

  // WAIT_UNTIL(false);
  
  // WAIT_UNTIL(!gyro.is_calibrating());
	// setFlywheelVel(2115);
	// driveBrake();
	// shoot(2);
  // shooter.waitToReachState(ShooterIdleParams{});
	// WAIT_UNTIL(false);
	
	// provSkills();
	// Auton::runAuton();
	// intakeOn();
	// WAIT_UNTIL(false);

	// provSkillsLow();
	// provSkills();
	// autonAWP4();

	// backupSkills();

	// Auton::runAuton();
	// provSkills();
}


Auton auton1("autonStack", autonStack);
Auton auton2("autonAWP", autonAWP5);
Auton auton3("autonLine", autonLine, E_Auton_Reset_Types::far);
Auton auton4("Skills", provSkillsLow);

// #define MAIN
// #define PROG_SKILLS
// #define DRIVER_SKILLS
// #define MATCH
// #define SELECT
#define util

/*
Auton CheckList:
	- Make sure discs/tiles are setup consistenly(Check game manual)
	- After every move, measure the position of the robot relative to one game element(disc) and the field permiter
		- Log this info so if it fails, you can compare it to previous things
	- Save multiple logs of working autons


Next year layout??:
	Async vs Sync:
		- For autons, all movement of the drivebase will be inline while all other subsystems will be tasks
		- For driver, the control of drivebase and buttons will be inline while all other subsystems will be tasks
	File Layout:
		Libraries(These have to be flawless - no issues occuring):
			- Geometry
			- Timer
			- Queue(fix)
			- PID
			- Util
			- Logging(change)
		Devices(These should just be wrappers):
			- Controller
			- Piston
			- Motors(maybe)
		- Main
		- Tracking
		- Drive(This will include motion algorithms)
		- Auton(Should include menu)
		- Config
		- ALL SUBSYSTEMS
	Testing tools:
		- logging
		- a controller print queue that scrolls through prints at a 250-500ms rate
		- Really good pre-auton notification/testing program
			- Checks sensors and motors
			- Checks for drift and zeroed gyro
			- Auton selector
		- settings.txt file in the sd that stores all the settings for drive code
			- a settings menu that allows driver to change these settings
			- Settings:
				- Turn speed
				- Slewing(if applicable)
				- Min turn speed
				- Dead zone

*/

// 100 = 36ms
// 1 = 2.6ms

/*
NOTES:
	- Write rotation sensor safety
	- Write menu to run quick turns, and motion algorithms(With images)

*/
#ifdef util
	namespace UtilProgram {
		struct Test {
			function<void()> test_function;
			std::string name;

			Test(function<void()> test_function, std::string name): test_function(test_function), name(name) {}

			void run_function() {
				test_function();
			}
		};

		struct TestingSubsection{
			std::vector<Test> test;
			std::string name;

			TestingSubsection(std::vector<Test> tests, std::string name) : test(tests), name(name) {}

			void print(int index){
				master.print(2, 0, "%s", test[(index-1)%(test.size())].name.c_str());
				master.print(1, 0, "%s---------------", test[(index)%(test.size())].name.c_str());
				master.print(0, 0, "%s", test[(index+1)%(test.size())].name.c_str());

				cout << (index)%(test.size()) << endl;
			}

			void selectTest(){
				std::string input; 
				int index = 1;

				print(index);

				while (true){ // not b is press
					
					if (master.get_digital_new_press(DIGITAL_UP)){
						index++;
						print(index);
					} else if (master.get_digital_new_press(DIGITAL_DOWN)){
						index--;
						print(index);
					} else if (master.get_digital_new_press(DIGITAL_A)){
						test[index%test.size()].run_function();
					} else if (master.get_digital_new_press(DIGITAL_B)){
						break;
					}

					delay(50);
				}
			}
		};

		void turn45() {
			cout << "Turning 45 degrees\n";
		}
		void turn90() {
			cout << "Turning 90 degrees\n";
		}
		void turn15() {
			cout << "Turning 15 degrees\n";
		}
		void turnX(){
			int degrees = 25;
			master.clear();
			master.print(0, 0, "Inches: %d", degrees);
			while (true){
				if (master.get_digital_new_press(DIGITAL_UP)) master.print(0, 0, "Inches: %d", degrees += 1);
				else if (master.get_digital_new_press(DIGITAL_DOWN)) master.print(0, 0, "Inches: %d", degrees -= 1);
				else if (master.get_digital_new_press(DIGITAL_LEFT)) master.print(0, 0, "Inches: %d", degrees -= 5);
				else if (master.get_digital_new_press(DIGITAL_RIGHT)) master.print(0, 0, "Inches: %d", degrees += 5);
				else if (master.get_digital_new_press(DIGITAL_A)) {moveForwardSync(degrees); break;}
				else if (master.get_digital_new_press(DIGITAL_B)) break;

				delay(50);
			}

			
		}


		void move70() {
			cout << "Moving 70 inches\n";
		}
		void move24() {
			cout << "Moving 24 inches\n";
		}
		void move10() {
			cout << "Moving 10 inches\n";
		}
		void moveX(){
			int inches = 25;
			master.clear();
			master.print(0, 0, "Inches: %d", inches);
			while (true){
				if (master.get_digital_new_press(DIGITAL_UP)) {inches++; master.print(0, 0, "Inches: %d", inches);}
				else if (master.get_digital_new_press(DIGITAL_DOWN)) {inches--; master.print(0, 0, "Inches: %d", inches);}
				else if (master.get_digital_new_press(DIGITAL_LEFT)) {inches -= 5; master.print(0, 0, "Inches: %d", inches);}
				else if (master.get_digital_new_press(DIGITAL_RIGHT)) {inches += 5; master.print(0, 0, "Inches: %d", inches);}
				else if (master.get_digital_new_press(DIGITAL_A)) {moveForwardSync(inches); break;}
				else if (master.get_digital_new_press(DIGITAL_B)) break;

				delay(50);
			}

			
		}

		void gyroCheck(){
			master.clear();
			WAIT_UNTIL(!gyro.is_calibrating());
			int drift_count = 0;
			while(true){
				if(fabs(radToDeg(tracking.g_vel.a)) > 0.01){
					drift_count++;
					if(drift_count > 10){
						master.rumble("........");
						master.print(0, 0, "DRIFT DETECTED");
					}
				} else if (master.get_digital_new_press(DIGITAL_B)){
					break;
				}
				delay(10);
			}
		}
		

		void run() {
			TestingSubsection turns{{{turn45, "Turn45"}, {turn90, "Turn90"}, {turn15, "turn15"}, {turnX, "turnX"}}, "Turns"};
			TestingSubsection moves{{{move70, "move70"}, {move24, "move24"}, {move10, "move10"}, {moveX, "moveX"}}, "Moves"};
			TestingSubsection checks{{{move70, "gyro_drift"}, {move24, "gyro_zero"}, {move10, "tracking_0"}}, "Checks"};
			

			std::string input;
			int index = 0;

			TestingSubsection test_sub[] = {turns, moves, checks};

			master.print(0, 0, "%s", test_sub[(index-1)%(sizeof(test_sub) / sizeof(test_sub[0]))].name.c_str());
			master.print(1, 0, "%s---------------", test_sub[(index  )%(sizeof(test_sub) / sizeof(test_sub[0]))].name.c_str());
			master.print(2, 0, "%s", test_sub[(index+1)%(sizeof(test_sub) / sizeof(test_sub[0]))].name.c_str());

			while (true){

				if (master.get_digital_new_press(DIGITAL_UP)){
					index++;
					master.print(2, 0, "%s", test_sub[(index-1)%(sizeof(test_sub) / sizeof(test_sub[0]))].name.c_str());
					master.print(1, 0, "%s---------------", test_sub[(index  )%(sizeof(test_sub) / sizeof(test_sub[0]))].name.c_str());
					master.print(0, 0, "%s", test_sub[(index+1)%(sizeof(test_sub) / sizeof(test_sub[0]))].name.c_str());
				} else if (master.get_digital_new_press(DIGITAL_DOWN)){
					index--;
					master.print(2, 0, "%s", test_sub[(index-1)%(sizeof(test_sub) / sizeof(test_sub[0]))].name.c_str());
					master.print(1, 0, "%s---------------", test_sub[(index  )%(sizeof(test_sub) / sizeof(test_sub[0]))].name.c_str());
					master.print(0, 0, "%s", test_sub[(index+1)%(sizeof(test_sub) / sizeof(test_sub[0]))].name.c_str());
				} else if (master.get_digital_new_press(DIGITAL_A)){
					test_sub[index%(sizeof(test_sub) / sizeof(test_sub[0]))].selectTest();
					master.print(2, 0, "%s", test_sub[(index-1)%(sizeof(test_sub) / sizeof(test_sub[0]))].name.c_str());
					master.print(1, 0, "%s---------------", test_sub[(index  )%(sizeof(test_sub) / sizeof(test_sub[0]))].name.c_str());
					master.print(0, 0, "%s", test_sub[(index+1)%(sizeof(test_sub) / sizeof(test_sub[0]))].name.c_str());
				}

				cout << (index+1)%(sizeof(test_sub) / sizeof(test_sub[0])) << endl;


				delay(50);
			}
			moves.selectTest();
		}

	}
#endif
void opcontrol() {
	
#ifdef MAIN
	

	WAIT_UNTIL(!gyro.is_calibrating());

	// spinRoller();
	// moveToTargetSync({0, 34});
	
	

	


	// while (true){
	// 	log("TEST");
	// 	delay(5);
	// }


	// _Task test_a;
	// test_a.start([](){
	// 	for(int i = 0; i < 100; i++){
	// 		for(int i = 0; i < 110; i++){
	// 			log("a");
	// 		}
	// 		log("\n");
	// 	}
	// });
	// Task test_b([](){
	// 	for(int i = 0; i < 100; i++){
	// 		for(int i = 0; i < 110; i++){
	// 			log("b");
	// 		}
	// 		log("\n");
	// 	}
	// });

	

#endif

#ifdef util
	UtilProgram::run();
	
#endif

#ifdef PROG_SKILLS
	WAIT_UNTIL(!gyro.is_calibrating());
	master.clear();
	tracking.reset({74.0, 8.0, M_PI_2});
	while(true){
		master.print(0, 0, "A: %.2lf", radToDeg(tracking.g_pos.a));
		delay(100);
	}

	WAIT_UNTIL(false);
#endif

#ifdef DRIVER_SKILLS
	setFlywheelVel(barrier_rpm - 50);
	shoot(9, true);	
	shooter.waitToReachState(ShooterIdleParams{});

	driverPractice();
#endif

#ifdef MATCH
	driverPractice();
#endif

#ifdef SELECT 
	Auton::selectAuton();
	WAIT_UNTIL(false);
#endif
}
