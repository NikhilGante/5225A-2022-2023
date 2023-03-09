#include "main.h"
#include "Libraries/gui.hpp"
#include "Libraries/logging.hpp"
#include "Devices/controller.hpp"
#include "Devices/others.hpp"
#include "Subsystems/intake.hpp"
#include "Subsystems/shooter.hpp"
#include "auton.hpp"
#include "config.hpp"
#include "menu.hpp"
#include "tracking.hpp"
#include "util.hpp"

#include <fstream>

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
  DEBUG;

  // Logging::init();
	_Controller::init();
  GUI::init();
  tracking.init({30.75, 9.0, degToRad(0.0)});	// ACTUAL SKILLS

  misc_checks.select();
	delay(300);
	drive.runMachine();
	intake.runMachine();
	flywheel.runMachine();
	shooter.runMachine();

  DEBUG;
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
  DEBUG;
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
  DEBUG;
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
  DEBUG;
	Auton::run();
	WAIT_UNTIL(!gyro.isCalibrating());
	autonAWP();
	// Auton::runAuton();
	// provSkills();
  DEBUG;
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

void auton2func() {auton_log("whatup");}
void auton3func() {auton_log("ayyyy");}

Auton auton1("Stack", autonStack);
Auton auton2("AWP", autonAWP);
Auton auton3("Line", autonLine, Auton::E_Reset_Types::far);
Auton auton4("Skills", provSkillsLow);

//!fix logging issue
//!check that log folders is good
//add rumble back to alert
//controller queue
//remove timer from subsystems by making them static, and then initializing in the constructor
//Add a config page to the gui
//Logging different sizes
//Eliminate the idea of multiple guis
//Make all buttons use a Text

timer.get_time
pros c u
fabs
Touch -> Light
lcd

//debugging doc
//change GitHub to conventions and styles doc

void opcontrol() {
  DEBUG;
  std::ofstream file_init{"/usd/testfile1.txt", std::ofstream::trunc};
  file_init << "Hello";
  file_init.close();
  
  DEBUG;
  WAIT_UNTIL(false){
    if(master.getNewDigital(DIGITAL_A)) alert::start(term_colours::MAGENTA, "Hey There!");
  }

  DEBUG;
}