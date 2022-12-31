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

#include "lift.hpp"
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
	// log_init();
	lcd::initialize();
	// tracking.g_pos = {29.25, 0.0, degToRad(0.0)};	// new_skills2

	tracking.g_pos = {30.75, 7.375, degToRad(0.0)};	// new_skills2

	// tracking.g_pos = {34.75, 11.25, degToRad(0.0)};	// newSkills1
	// tracking.g_pos = {108.0, 129.75, degToRad(180.0)};	// new_skills2
	// tracking.g_pos = {11.25, 23.5, degToRad(90.0)};	// new_skills3
	// tracking.g_pos = {129.75, 116.5, degToRad(-90.0)}; // new_skills4


	// tracking.g_pos = {35.25, 11.25, 0.0};	// skills1
	// tracking.g_pos = {68.00, 129.25, M_PI};	// skills2
	// tracking.g_pos = {72.0, 11.25, 0.0};	// skills3

	// tracking.g_pos = {0.0, 0.0, 0.0};

	log_init();
	_Task tracking_task("tracking_update_task");
	tracking_task.start(trackingUpdate);
	// Data::init();
	_Controller::init();
	delay(300);
	// lift.runMachine();
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
	new_skills2();

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


// start (70, 129.5)
// end: (71, 11.5)

// coords of high goal (approx)
// Red: (18.0, 123.0)

// STACK TRACE

void moveInchesStraight(double target){
	Timer move_timer{"move_timer"};
	double start_l = left_tracker.get_position()*1/36000.0 *(2.75*M_PI);
	double start_r = right_tracker.get_position()*1/36000.0 *(2.75*M_PI);
	double error, error_a;
	double cur_l, cur_r;
	double power;
	do {
		cur_l = left_tracker.get_position()*1/36000.0 *(2.75*M_PI) - start_l;
		cur_r = right_tracker.get_position()*1/36000.0 *(2.75*M_PI) - start_r;

		error_a = (cur_l-cur_r)*2;
		error = target-(cur_l+cur_r)/2;
		power = error*5.0;

		if(fabs(power) < 30) power = sgn(error) * 30;

		moveDrive(power, error_a);

	} while (fabs(error) > 0.5);
	master.print(2, 0, "time: %ld", move_timer.getTime());
	driveBrake();
	master.rumble("-");
}

void moveInches(double target){
	Timer move_timer{"move_timer"};
	double start = left_tracker.get_position()*1/36000.0 *(2.75*M_PI);
	double error;
	do{
		double cur_y = left_tracker.get_position()*1/36000.0 *(2.75*M_PI) - start;
		error = target - cur_y;
		double power = 5.0*error;
		if(fabs(power) < 30) power = sgn(error) * 30;
		// if(fabs(power) > 100) power = sgn(error) * 100;
		moveDrive(power, 0.0);

	}while(fabs(error) > 0.5);
	master.print(2, 0, "time: %ld", move_timer.getTime());
	driveBrake();
	master.rumble("-");
}
// acc: 

// tracking:
// (69.75, 134.625, 180)
void autonProgram(){

}
pros::Distance ds(20);
auton auton1("auton1", 5, 20, 0, autonProgram, ds);
auton auton2("auton2", 20, 20, 0, autonProgram, ds);
auton auto3("auton3", 20, 10, 0, autonProgram, ds);




void opcontrol() {
	auton::program1();
	
	
	roller_sensor.set_led_pwm(100);

	pros::c::optical_rgb_s_t rgb_value;
	while (1){
		roller_sensor.set_led_pwm(100);
		rgb_value = roller_sensor.get_rgb();
		printf("%lf \n", rgb_value.red);
		delay(100);
	}


	WAIT_UNTIL(false);


	// setFlywheelVel(2320);

	Timer disc_count_print{"disc_count_print"};
	Timer angle_override_print{"angle_override_print"};
	master.clear();
	while(true){

		driveHandleInput();
		shooterHandleInput();
		intakeHandleInput();
		// if(master.get_digital_new_press(DIGITAL_X)){
		// 	moveInches(50.0);
		// 	turnToAngleSync(-90.0);
		// } 
		//  && g_mag_disc_count <= 4
		// && g_mag_disc_count >= 0
		if(master.get_digital_new_press(DIGITAL_UP) || partner.get_digital_new_press(DIGITAL_UP))	g_mag_disc_count++;
		if(master.get_digital_new_press(DIGITAL_DOWN) || partner.get_digital_new_press(DIGITAL_DOWN))	g_mag_disc_count--; 


		if(disc_count_print.getTime() > 100){
			master.print(0,0, "disc count: %d  ", g_mag_disc_count.load());
			// partner.print(0,0, "disc count: %d  ", g_mag_disc_count.load());
			disc_count_print.reset();
		}

		if(angle_override_print.getTime() > 100){
			angle_override_print.reset();
			if (angleOverride) master.print(1, 0, "Override");
			else master.print(1, 0, "Automatic");
		}

		// if(centre_l.get_temperature() >= 50 || centre_r.get_temperature() >= 50 || intake_m.get_temperature() >= 50 || flywheel_m.get_temperature() >= 50){
		// 	break;
		// }
		delay(10);
	}


	// while(true){
	// 	if(master.get_digital_new_press(DIGITAL_A)) indexer_p.toggleState();
	// 	delay(10);
	// }

	// moveInches(20.0);
	Timer timer1{"timer"};
	angler_p.setState(LOW);
	moveToTargetSync({30.75, 12.5});
	aimAtBlue(10.0);
	delay(2000);
	Timer shoot_timer{"timer"};
	shoot(2);
	shooter.waitToReachState(ShooterIdleParams{});
	lcd::print(5, "shoot_time:%ld", shoot_timer.getTime());
	setFlywheelVel(2320);
	turnToTargetSync({69.0, 45.0});
	tracking.waitForDistance(15.0);
	intakeOn();
	moveToTargetSync({69.0, 45.0}, E_Brake_Modes::brake, 55);
	aimAtBlue(10.5);
	shoot(3);
	turnToTargetSync({126.0, 111.0});
	moveToTargetSync({126.0, 111.0});

	turnToAngleSync(-90.0);
	lcd::print(6, "total:%ld", timer1.getTime());

	// turnToAngleSync(179.0);
	// moveInches(120.0);
	// turnToAngleSync(180.0);
	// flattenAgainstWallSync(true);
	// while(true){
	// 	// if(master.get_digital_new_press(DIGITAL_Y))	moveInches(10.0);
	// 	driveHandleInput();
	// 	intakeHandleInput();
	// 	delay(10);
	// }
	// WAIT_UNTIL(false);
	// Timer disc_count_print{"g_mag_disc_count_print"};
	// moveDrive(0, 127);
	WAIT_UNTIL(false);
	master.print(0,0, "curvature: %.2lf", angle_curvature);
	driveBrake();
	while(true){
		driveHandleInput();
		shooterHandleInput();
		intakeHandleInput();

		// if(disc_count_print.getTime() >= 50){
		// 	master.print(0,0, "disc count: %d", g_mag_disc_count.load());
		// 	disc_count_print.reset();
		// }
		lcd::print(7, "flywheel:%.lf", flywheel_m.get_temperature());
		if(centre_l.get_temperature() >= 50 || centre_r.get_temperature() >= 50 || intake_m.get_temperature() >= 50 || flywheel_m.get_temperature() >= 50){
			break;
		}
		delay(10);
	}
	moveDrive(0, 0);
	intake_m.move(0);
	flywheel_m.move(0);
	master.rumble("----");
	// turnToAngle(90.0);

	WAIT_UNTIL(false);
	bool intk_on = true;
	while(true){
		if(master.get_digital_new_press(DIGITAL_A)) intk_on = !intk_on;
		if(intk_on) intake_m.move(127);
		else intake_m.move(0);
		delay(10);
	}

	tracking.reset();
	// intake_m.move(127);
	delay(300);
	// Timer timer{"timer"};
	// moveDrive(37,0);
	// WAIT_UNTIL(tracking.g_pos.y > 20.0);
	// driveBrake();
	// lcd::print(6, "time %d", timer.getTime());
	// WAIT_UNTIL(false);
	bool pressed = false, pressed_last = false;
	bool pressedy = false, pressed_lasty = false;
	bool printed = false;
	// WAIT_UNTIL(master.get_digital(DIGITAL_B));
	Timer timer{"timer"};
	while(true){
		pressedy = master.get_digital(DIGITAL_Y);

		// if(ma)	tracking.reset()

		driveHandleInput();
		if(intk_on) intake_m.move(127);
		else intake_m.move(0);

		pressed = master.get_digital(DIGITAL_A);
		// if(master.get_digital_new_press(DIGITAL_B))	intk_on = !intk_on;
		if(pressedy && !pressed_lasty){
			printed = false;
			tracking.reset();
		}	

		if(pressed && !pressed_last)	intk_on = !intk_on;



		pressed_last = pressed;
		pressed_lasty = pressedy;
		printf("%d\n", master.get_digital(DIGITAL_UP));

		// if(tracking.g_pos.y > 50 && !printed){
		// 	printed = true;
		// 	lcd::print(6, "time %d", timer.getTime());
		// 	master.print(0, 1, "time %d", timer.getTime());
		// 	driveBrake();
		// 	delay(800);
		// }		


		// moveDriveSide(master.get_analog(ANALOG_LEFT_Y), master.get_analog(ANALOG_RIGHT_Y));
		lcd::print(5, "%lf %lf %lf", centre_l.get_temperature(), centre_r.get_temperature(), intake_m.get_temperature());
		if(centre_l.get_temperature() >= 45 || centre_r.get_temperature() >= 45 || intake_m.get_temperature() >= 45) break;
		
		delay(10);
	}
	moveDrive(0, 0);
	intake_m.move(0);
	flywheel_m.move(0);
	driveBrake();
	master.rumble("---");

	WAIT_UNTIL(false);
	// while(true){
	// 	handleInput();
	// 	delay(10);
	// }
	master.clear();
	delay(150);
	master.print(0,  0, "%d", 20);
	master.print(1, 2, "%d", 20);
	master.print(2, 0, "%d", 20);

	master.print(0,  0, "%d", 40);
	master.rumble("-");

}
