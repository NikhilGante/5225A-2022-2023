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

const GUI* GUI::current_gui = &util_obj;


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
 
void opcontrol() {
  // data1.log_file.open("/usd/log.txt");
  // // for(int i = 0; i < 100; i++){
	// // 	for(int j = 0; j < 5; j++){	
	// // 		data1.print("aaaaaaaaaa");
	// // 	}
	// // 	// data.print("\n");
	// // 	data1.print("\n");
	// // 	// delay(1);
	// // }

  // data1.print("abcdefghijklmnopqrstuvwxyz\n");

  const int intk_port = 20, speed = 127;
  char pneum_port = 'H';
  std::uint32_t delay_time = 65;

  WAIT_UNTIL(false){
    if(master.get_digital_new_press(DIGITAL_X)) c::motor_move(intk_port, 0);
    else if(master.get_digital_new_press(DIGITAL_UP)) c::motor_move(intk_port, -speed);
    else if(master.get_digital_new_press(DIGITAL_DOWN)) c::motor_move(intk_port, speed);

    if(master.get_digital_new_press(DIGITAL_L1)){
      printf("Config:%d\n", c::adi_port_set_config(pneum_port, E_ADI_DIGITAL_OUT));
      c::adi_port_set_value(pneum_port, LOW);
      delay(delay_time);
      c::adi_port_set_value(pneum_port, HIGH);
    }
    else if(master.get_digital_new_press(DIGITAL_R1)){
      printf("Config:%d\n", c::adi_port_set_config(pneum_port, E_ADI_DIGITAL_OUT));
      for(int i = 0; i < 3; i++){
        c::adi_port_set_value(pneum_port, LOW);
        delay(delay_time);
        c::adi_port_set_value(pneum_port, HIGH);
        delay(275);
      }
    }
  }
  
}