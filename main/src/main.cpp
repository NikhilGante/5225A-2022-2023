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


	// drive.runMachine();
	// intake.runMachine();
	// flywheel.runMachine();
	// shooter.runMachine();
	
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

  WAIT_UNTIL(!gyro.is_calibrating());
  int drift_count = 0;
  while(true){
    if(fabs(radToDeg(tracking.g_vel.a)) > 0.01){
      drift_count++;
      if(drift_count > 10){
        master.rumble("........");
        master.print(0, 0, "DRIFT DETECTED");
      }
    }
    delay(10);
  }

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

#define MAIN
// #define PROG_SKILLS
// #define DRIVER_SKILLS
// #define MATCH
// #define SELECT

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
void opcontrol() {
#ifdef MAIN


	while (true){
		log("TEST");
		delay(5);
	}
	

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
