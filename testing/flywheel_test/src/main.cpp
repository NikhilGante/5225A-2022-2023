#include "main.h"
#include "Libraries/piston.hpp"
using namespace pros;

Motor flywheel_m(5, E_MOTOR_GEARSET_06);
Motor intake_m(20, E_MOTOR_GEARSET_18);
Piston indexer_p('E', "indexer_p", true, LOW);

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	pros::lcd::initialize();

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


void opcontrol() {
	pros::Controller master(pros::E_CONTROLLER_MASTER);
	pros::Motor left_mtr(1);
	pros::Motor right_mtr(2);
	int flywheel_power = 127;

	// while(true){
	// 	if(master.get_digital_new_press(DIGITAL_A)){	// toggle state of subsystem
	// 		indexer_p.setState(!indexer_p.getState());
	// 	}
	// 	if (indexer_p.getState())	printf("HIGH\n");
	// 	else	printf("LOW\n");
		
	// 	delay(10);
	// }

	Task flywheel_t([&](){
		intake_m.move(-127);

		int shots_left = 0;

		while(true){
			if(master.get_digital_new_press(DIGITAL_A))	shots_left = 3;	// Press A to shoot 3
			if(master.get_digital_new_press(DIGITAL_X))	shots_left = 1;	// Press X to shoot 1
			if(shots_left > 0){
					printf("STARTED SHOOTING\n");
					indexer_p.setState(HIGH);	
					delay(75); // wait for SHOOTER to extend
					// delay(175);
					printf("FINISHED SHOT\n");
					indexer_p.setState(LOW);
					// delay(100);// wait for SHOOTER to retract
					delay(175);// wait for SHOOTER to retract
					printf("FINISHED Retraction\n");
					shots_left--;
			}
			delay(10);
		}
	});

	uint32_t print_timer = millis();
	while (true) {
		if(master.get_digital_new_press(DIGITAL_UP))	flywheel_power = std::clamp(flywheel_power + 5, 0, 127);
		if(master.get_digital_new_press(DIGITAL_DOWN))	flywheel_power = std::clamp(flywheel_power - 5, 0, 127);
		flywheel_m.move(flywheel_power);
		if(print_timer - millis() > 50){
			printf("%d, %.lf\n", millis(), 5*flywheel_m.get_actual_velocity());
			lcd::print(0, "power:%d vel:%.lf temp:%.lf", flywheel_power, 5*flywheel_m.get_actual_velocity(), flywheel_m.get_temperature());
			print_timer = millis();
		}

		// Motor temp safety
		if(intake_m.get_temperature() > 45 || flywheel_m.get_temperature() > 45){
			intake_m.move(0);
			flywheel_m.move(0);
			break;
		}
		delay(10);
	}
}
