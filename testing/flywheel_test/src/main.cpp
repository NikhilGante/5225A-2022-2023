#include "main.h"
#include <atomic>
#include "Libraries/piston.hpp"
#include "pros/adi.hpp"

using namespace pros;
using namespace std;

pros::Controller master(pros::E_CONTROLLER_MASTER);
Motor flywheel_m(9, E_MOTOR_GEARSET_06, false, pros::E_MOTOR_ENCODER_DEGREES);
Motor intake_m(20, E_MOTOR_GEARSET_18);

// old piston code
// original fail - true HIGH
/* tried:
	reverse - (open), init
	true HIGH	(flicked back and forth)
	true LOW	extended once
	false LOW	extended once
	false HIGH (flick)
*/

// original fail - true HIGH
/* tried:
	true LOW	(flick)
	false LOW	extended once
	false HIGH (flick)
*/

atomic<int> g_mag_disc_count = 0;


Piston indexer_p('E', "indexer_p", false, LOW);
ADIAnalogIn flywheel_ds('H');

ADIAnalogIn intk_ds('A');
ADIAnalogIn mag_ds('G');

enum class E_Intake_States{
	off,
	on,
	rev
};
E_Intake_States intake_state = E_Intake_States::rev;

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

			// If mag is full, don't let any more discs in
			if(g_mag_disc_count >= 3){
				intake_state = E_Intake_States::rev;
				// delay(100);	// wait for third disc to enter mag before reversing
			}
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
	delay(300); // Check if sensor returns values above 0 for the first 100 ms

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

// high seems to be default state for piston

void opcontrol() {
	// disk counting code

	// intk: 2640 normal, check for less than 2500
	// mag: 800 normal, check for less than 500

/*
	int intk_ds_val, mag_ds_val;
	const int mag_disc_thresh = 500;
	bool mag_disc_detected = false, mag_disc_detected_last = false; // if disc is currently detected by mag sensor

	while(true){
		intakeHandle();
		intk_ds_val = intk_ds.get_value(), mag_ds_val = mag_ds.get_value();
		mag_disc_detected = mag_ds_val < mag_disc_thresh;

		if(intake_m.get_direction()){	// if intake moving forwards
			if(!mag_disc_detected && mag_disc_detected_last){	// disk just now left mag sensor (entered mag)
				g_mag_disc_count++;
				printf("INCR\n");
			}
		}

		mag_disc_detected_last = mag_disc_detected;
		if(master.get_digital_new_press(DIGITAL_LEFT)) g_mag_disc_count = 0;


		printf("%d %d %d %d\n", millis(), intk_ds_val, mag_ds_val, g_mag_disc_count.load());
		delay(10);
	}
*/

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
	atomic<int> shooting = 0;
	pros::Rotation rotation_sensor(10);
	rotation_sensor.set_data_rate(5);	// Gets data from rotation sensor every "5" - actually 10ms
	rotation_sensor.reset_position();
	int vel_target = 2500;
	long rot_vel;

	double kB = 0.0385;	// Target velocity multiplied by this outputs a motor voltage
	double kP = 0.35;
	double proportional;
	double output; // What power goes to the flywheel motor

	uint32_t print_timer = millis();


	Task flywheel_t([&](){
		int shots_left = 0;
		uint32_t shoot_timer = millis();
		while(true){
			if(master.get_digital_new_press(DIGITAL_A))	shots_left = 3;	// Press A to shoot 3
			if(master.get_digital_new_press(DIGITAL_X))	shots_left = 1;	// Press X to shoot 1
			if(millis() - shoot_timer > 300 && fabs(error.load() < 20) && shots_left > 0){
					printf("%d STARTED SHOOTING\n", millis());
					shooting = 1000;
					indexer_p.setState(HIGH);	
					delay(75); // wait for SHOOTER to extend
					// delay(175);
					printf(" %d FINISHED SHOT\n", millis());
					shooting = 0;
					indexer_p.setState(LOW);
					// delay(100);// wait for SHOOTER to retract
					delay(175);// wait for SHOOTER to retract
					printf("%d FINISHED Retraction\n", millis());
					shots_left--;
					shoot_timer = millis();
			}
			delay(10);
		}
	});
	master.clear();
	lcd::print(4, "Ports | intk:%d fly:%d pist:E rot:1", intake_m.get_port(), flywheel_m.get_port());
	lcd::print(5, "Btns | 3 shots:A, 1 shot:X");
	lcd::print(6, "Btns | intake toggle:Y, rev: B");
	lcd::print(7, "Press up/down to change fly vel");

	// const double filter_val = 0.0;
	double last_vel;

	const double smooth_val = 0.65;

	double pos = 0, last_pos = 0;
	double manual_vel;
	double smoothed_vel = 0;

	uint32_t motor_vel_read_time = millis();
	while (true) {
		intakeHandle();
		// if(master.get_digital_new_press(DIGITAL_UP))	flywheel_power = std::clamp(flywheel_power + 5, 0, 127);
		// if(master.get_digital_new_press(DIGITAL_DOWN))	flywheel_power = std::clamp(flywheel_power - 5, 0, 127);
		rot_vel = 3*60*rotation_sensor.get_velocity()/360;	// actual velocity of flywheel
		// rot_vel = filter_val*last_vel + (1-filter_val)*rot_vel;
		// last_vel = rot_vel;

		error = vel_target - smoothed_vel;
		proportional = kP * error;
		// if(fabs(error) > 50)	proportional = kP * error;
		// else	proportional = 0.2 * error;
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

		if(millis() - motor_vel_read_time >= 40){
			pos = 5*flywheel_m.get_position()/360;
			manual_vel = 60000 * (pos - last_pos)/(millis() - motor_vel_read_time);
			last_pos = pos;

			smoothed_vel = manual_vel*(1-smooth_val) + last_vel*smooth_val;
			last_vel = smoothed_vel;

			motor_vel_read_time = millis();
			// printf("%d, diff: %lf, time_diff: %d, motor_vel:%lf", millis(), pos - last_pos, (millis() - motor_vel_read_time), motor_vel);
		}
		printf("%d, %d, %d, %ld, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf, %.2lf\n", millis(), flywheel_ds.get_value(), vel_target, rot_vel, error.load(), output, vel_target * kB, proportional, manual_vel, smoothed_vel);
	
		// flywheel_m.move(0);
		flywheel_m.move(output);
		// flywheel_m.move(127);
		
		if(print_timer - millis() > 50){
			// printf("%d, %.lf\n", millis(), 5*flywheel_m.get_actual_velocity());
			lcd::print(0, "power:%.lf vel:%.lf temp:%.lf", output, rot_vel, flywheel_m.get_temperature());
			lcd::print(1, "intake temp:%.lf", intake_m.get_temperature());

			// lcd::print(0, "power:%d vel:%.lf temp:%.lf", output, 5*flywheel_m.get_actual_velocity(), flywheel_m.get_temperature());
			print_timer = millis();
		}

		// Motor temp safety
		// if(intake_m.get_temperature() >= 45){
		// 	intake_m.move(0);
		// 	flywheel_m.move(0);
		// 	break;
		// }
		delay(10);
	}
	master.clear(); // Rumble to let us know safety has triggered 
	delay(100);
	master.print(0, 0, "yo");
	while(true){
		delay(10);
	}
}
