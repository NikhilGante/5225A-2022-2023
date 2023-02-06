#include "main.h"
#include "Libraries/gui.hpp"
#include "Libraries/task.hpp"
#include "Libraries/logging.hpp"
#include "Devices/controller.hpp"
#include "auton.hpp"
#include "config.hpp"
#include "menu.hpp"
#include "tracking.hpp"
#include "Devices/vision.hpp"

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

	tracking.reset({30.75, 9.0, degToRad(0.0)});	// ACTUAL SKILLS
	_Task tracking_task("Tracking Update");
	tracking_task.start(trackingUpdate);

  misc_checks.select();
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
	Auton::run();
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

void auton2func(){
	auton_log("whatup");
}

void auton3func(){
	auton_log("ayyyy");
}

Auton auton1("autonStack", autonStack);
Auton auton2("autonAWP", autonAWP);
Auton auton3("autonLine", autonLine);
Auton auton4("Skills", fullSkills);

//TODO: Test log to terminal


void print_object(vision_object_s_t obj){
  constexpr int deadzone = 35;
  std::string type;
  switch(obj.type){
    case VISION_OBJECT_NORMAL: type = "Normal"; break;
    case VISION_OBJECT_COLOR_CODE: type = "Colour"; break;
    case VISION_OBJECT_LINE: type = "Line"; break;
  }
  _Vision::print_object(obj);
  if(std::abs(obj.left_coord) > deadzone) printf2("    Go %s", obj.left_coord > 0 ? "Right" : "Left");
  else if(std::abs(obj.top_coord) > deadzone) printf2("    Go %s", obj.top_coord > 0 ? "Up" : "Down");
  
  printf2("\n");
}

void opcontrol() {
  DEBUG;
  _Vision vis(2);

  while(true) {
    vision_object_s_t blue = vis.blue_object();
    vision_object_s_t red = vis.red_object();
    // printf2("Blue: %d, Red: %d\n", blue.signature, red.signature);
    if (red.signature != 255) print_object(red);
    delay(10);
  }

  
  DEBUG;
  WAIT_UNTIL(false);
}