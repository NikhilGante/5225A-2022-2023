#include "main.h"
#include <atomic>
#include "Libraries/piston.hpp"

using namespace pros;
using namespace std;

pros::Controller master(pros::E_CONTROLLER_MASTER);
Motor flywheel_m(5, E_MOTOR_GEARSET_06);
Motor intake_m(20, E_MOTOR_GEARSET_18);
Piston indexer_p('E', "indexer_p", true, HIGH);
ADIAnalogIn disc_sensor('G');

enum class E_Intake_States{
	off,
	on,
	rev
};
E_Intake_States intake_state = E_Intake_States::on;

void intakeHandle(){
	switch (intake_state) {
		case E_Intake_States::off:
			intake_m.move(0);
			if(master.get_digital_new_press(DIGITAL_Y)) intake_state = E_Intake_States::on;
			if(master.get_digital_new_press(DIGITAL_B)) intake_state = E_Intake_States::rev;
			break;
		case E_Intake_States::on:
			intake_m.move(127);
			if(master.get_digital_new_press(DIGITAL_Y)) intake_state = E_Intake_States::off;
			if(master.get_digital_new_press(DIGITAL_B)) intake_state = E_Intake_States::rev;
			break;
		case E_Intake_States::rev:
			intake_m.move(-127);
			if(master.get_digital_new_press(DIGITAL_Y)) intake_state = E_Intake_States::off;
			break;
	}
}

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
	// while(true){
	// 	flywheel_m.move(master.get_analog(ANALOG_LEFT_Y));
	// 	delay(10);
	// }
	// int flywheel_power = 127;

	// while(true){
	// 	if(master.get_digital_new_press(DIGITAL_A)){	// toggle state of subsystem
	// 		indexer_p.setState(!indexer_p.getState());
	// 	}
	// 	if (indexer_p.getState())	printf("HIGH\n");
	// 	else	printf("LOW\n");
		
	// 	delay(10);
	// }

	atomic<double> error;
	pros::Rotation rotation_sensor(1);	// Configures rotation sensor in port 5
	rotation_sensor.set_data_rate(5);	// Gets data from rotation sensor every "5" - actually 10ms
	rotation_sensor.reset_position();
	int vel_target = 2070;
	long rot_vel;

	double kB = 0.0385;	// Target velocity multiplied by this outputs a motor voltage
	double kP = 0.5;
	double proportional;
	double output; // What power goes to the flywheel motor

	uint32_t print_timer = millis();


	Task flywheel_t([&](){
		int shots_left = 0;

		while(true){
			if(master.get_digital_new_press(DIGITAL_A))	shots_left = 3;	// Press A to shoot 3
			if(master.get_digital_new_press(DIGITAL_X))	shots_left = 1;	// Press X to shoot 1
			if(shots_left > 0){
					printf("%d STARTED SHOOTING\n", millis());
					indexer_p.setState(HIGH);	
					delay(75); // wait for SHOOTER to extend
					// delay(175);
					printf(" %d FINISHED SHOT\n", millis());
					indexer_p.setState(LOW);
					// delay(100);// wait for SHOOTER to retract
					delay(175);// wait for SHOOTER to retract
					printf("%d FINISHED Retraction\n", millis());
					shots_left--;
			}
			delay(10);
		}
	});
	master.clear();
	lcd::print(4, "Ports | intk:%d fly:%d pist:E rot:1", intake_m.get_port(), flywheel_m.get_port());
	lcd::print(5, "Btns | 3 shots:A, 1 shot:X");
	lcd::print(6, "Btns | intake toggle:Y, rev: B");
	lcd::print(7, "Press up/down to change fly vel");
	
	while (true) {
		intakeHandle();
		// if(master.get_digital_new_press(DIGITAL_UP))	flywheel_power = std::clamp(flywheel_power + 5, 0, 127);
		// if(master.get_digital_new_press(DIGITAL_DOWN))	flywheel_power = std::clamp(flywheel_power - 5, 0, 127);
		rot_vel = 3*60*rotation_sensor.get_velocity()/360;	// actual velocity of flywheel

		error = vel_target - rot_vel;
		proportional = kP * error;
		output = vel_target * kB + proportional;
		output = std::clamp(output, -1.0, 127.0);	// decelerates at -1.0 at the most

		if(master.get_digital_new_press(DIGITAL_UP)){	// Increment the flywheel speed
			vel_target += 20;
			if(vel_target > 3000) vel_target = 3000;
			lcd::print(2, "vel_target:%d ", vel_target);
		}
		if(master.get_digital_new_press(DIGITAL_DOWN)){	// Decrement the flywheel speed 
			vel_target -= 20;
			if(vel_target < 0) vel_target = 0;
			lcd::print(2, "vel_target:%d ", vel_target);
		}

		printf("%d, %d, %d, %ld, %.2lf, %.2lf, %.2lf, %.2lf\n", millis(), disc_sensor.get_value(), vel_target, rot_vel, error.load(), output, vel_target * kB, proportional);
		
		flywheel_m.move(output);
		if(print_timer - millis() > 50){
			// printf("%d, %.lf\n", millis(), 5*flywheel_m.get_actual_velocity());
			lcd::print(0, "power:%.lf vel:%.lf temp:%.lf", output, rot_vel, flywheel_m.get_temperature());
			lcd::print(1, "intake temp:%.lf", intake_m.get_temperature());

			// lcd::print(0, "power:%d vel:%.lf temp:%.lf", output, 5*flywheel_m.get_actual_velocity(), flywheel_m.get_temperature());
			print_timer = millis();
		}

		// Motor temp safety
		if(intake_m.get_temperature() >= 45 || flywheel_m.get_temperature() >= 45){
			intake_m.move(0);
			flywheel_m.move(0);
			break;
		}
		delay(10);
	}
	master.rumble("-.-.-"); // Rumble to let us know safety has triggered 
}
