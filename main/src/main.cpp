#include "main.h"
#include "Libraries/gui.hpp"
#include "Libraries/task.hpp"
#include "Libraries/logging.hpp"
#include "Libraries/controller.hpp"
#include "auton.hpp"
#include "config.hpp"
#include "menu.hpp"
#include "tracking.hpp"

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

//TODO: Check //? across files
//TODO: Use Counter for Auton
//TODO: Test log to terminal
//TODO: Check if all ports are pluuged in


// void print_object(vision_object_s_t obj){
//   constexpr int deadzone = 35;
//   std::string type;
//   switch(obj.type){
//     case VISION_OBJECT_NORMAL: type = "Normal"; break;
//     case VISION_OBJECT_COLOR_CODE: type = "Colour"; break;
//     case VISION_OBJECT_LINE: type = "Line"; break;
//   }
//   printf2("Time:%d | Type:%s, Left:%d, Top:%d, Width:%d, Height:%d, Angle:%d", millis(), type, obj.left_coord, obj.top_coord, obj.width, obj.height, obj.angle);
//   if(std::abs(obj.left_coord) > deadzone) printf2("    Go %s", obj.left_coord > 0 ? "Right" : "Left");
//   else if(std::abs(obj.top_coord) > deadzone) printf2("    Go %s", obj.top_coord > 0 ? "Up" : "Down");
  
//   printf2("\n");
// }

void opcontrol() {
  DEBUG;
  // Vision vis(2, VISION_ZERO_CENTER);
  // // vis.set_auto_white_balance(true);

  // vision_signature_s_t blue_sig = Vision::signature_from_utility(1, -2835, -983, -1909, 6951, 9185, 8068, 3.0, 0);
  // vision_signature_s_t red_sig = Vision::signature_from_utility(2, 10795, 13423, 12109, -399, 809, 205, 3.0, 0);
  // vis.set_signature(blue_sig.id, &blue_sig);
  // vis.set_signature(red_sig.id, &red_sig);

  // while(true) {
  //   vision_object_s_t blue = vis.get_by_sig(0, 1);
  //   vision_object_s_t red = vis.get_by_sig(0, 2);
  //   // printf2("Blue: %d, Red: %d\n", blue.signature, red.signature);
  //   if (red.signature != 255) print_object(red);
  //   delay(10);
  // }

  
  DEBUG;
  WAIT_UNTIL(false);
}