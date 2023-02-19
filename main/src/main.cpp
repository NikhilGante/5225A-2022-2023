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
	lcd::initialize();

	tracking.g_pos = {0.0, 0.0, 0.0};
	// tracking.g_pos = {30.75, 9.0, degToRad(0.0)};	// ACTUAL SKILLS
	// tracking.g_pos = {128.5, 83.5, degToRad(0.0)};	// Line auton

	log_init();
	gyro.reset(true);
	_Task tracking_task("tracking_update_task");
	tracking_task.start(trackingUpdate);

	/*
	_Task battery_check("Battery_Checker");
	battery_check.start([](){
		while(true){
			// if(pros::battery::get_voltage()){master.}
			// printf("%d\n\n\n", pros::c::battery_get_voltage());
			if(pros::c::battery_get_voltage() < 12200){
				master.rumble("-");
			}
			delay(2000);

		}
	});
	*/
	
	// _Task led_task("led_task");
	// led_task.start([](){
	// 	while(true){
	// 		roller_sensor.set_led_pwm(100);
	// 		_Task::delay(100);
	// 	}
	// });

	// Data::init();
	_Controller::init();
	delay(300);
	// lift.runMachine();

	// drive.runMachine();
	intake.runMachine();
	flywheel.runMachine();
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
	// Auton::runAuton();
	WAIT_UNTIL(!gyro.is_calibrating());
	provSkills();
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


// acc: 

// tracking:
// (69.75, 134.625, 180)

// 2600, 2000 red
// 350, 200 blue
// THRESH: 1000

// 2900 red
// 420 blue

//1/4/2023
// red: 4100-5600
// blue: 1170
// thresh 3000

void auton1func(){
	printf("yooo\n");
}

void auton2func(){
	printf("whatup\n");
}

void auton3func(){
	printf("ayyyy\n");
}

Auton auton1("autonStack", autonStack);
Auton auton2("autonAWP", autonAWP);
Auton auton3("autonLine", autonLine, E_Auton_Reset_Types::far);
Auton auton4("Skills", fullSkills);



// thinks: (133.69, 122.40, -90.0) 
// at: (133.75 , 112.25, -90.0)


void opcontrol() {
	// flywheel.runMachine();
	// flywheel_m.move(0);
	// delay(100);
	// flywheel_m.move(50);

	// tracking.reset();

	driverPractice();



	// flattenAgainstWallSync();
	// tracking.reset({getDistL(), 9.75, degToRad(0.0)});
	// moveDrive(0, 0);
	// trans_p.setState(HIGH);
	
	WAIT_UNTIL(master.get_digital_new_press(DIGITAL_A));
	
	// drive.changeState(DriveIdleParams{});
  	// drive.waitToReachState(DriveIdleParams{});
	// moveDrive(0, -45);

	// turnToAngleSync(14);
	// driverPractice();
	WAIT_UNTIL(false);

  // drive.changeState(DriveIdleParams{});
  // drive.waitToReachState(DriveIdleParams{});
  // moveDrive(-50, 0);
	// while(true){
  //   log("%d l:%lf, r:%lf\n", millis(), tracking.l_vel, tracking.r_vel);
	// 	delay(10);
	// }
  // moveDrive(-10, 0);

	Auton::selectAuton();
	WAIT_UNTIL(master.get_digital_new_press(DIGITAL_A));
	Auton::runAuton();

	WAIT_UNTIL(false);
	// turnToAngleSync(45);


	driverPractice();

	// intake_m.move(-127);
	//  do{
	// 	roller_sensor.set_led_pwm(100);
  //   double cur_val = roller_sensor.get_rgb().red;
  //   log("%lf \n", roller_sensor.get_rgb().red, roller_sensor.get_rgb().blue);
  //   _Task::delay(100);
  // }while(true);


	// skills1();
	// skills2();
	// skills3();
	// WAIT_UNTIL(false);
	// THIS BY ITSELF
	// /*
		// Auton::selectAuton();
	// */
	WAIT_UNTIL(false);
	// spinRoller();
	// intake.waitToReachState(IntakeOffParams{});
	// tracking.reset({getDistL(), 7.5, 0.0});
	// while(true){
	// 	printf("dist:%lf | %lf\n", 141 - getDistL(), getDistR());
	// 	delay(10);
	// }


	// autonLine();

	// skills3();


	// E - angler_p is single
	// C - transmission is double

	/* 
	To fill:
		single: HIGH, double: LOW
	To Hold:
		single: LOW, double: LOW
	To Release:
		single: LOW, double: HIGH
	*/




	
	master.clear();
	// master.print(0,0, "press a to shoot");
	// endgame_s_p.setState(HIGH);
	// while(true){
	// 	// if(master.get_digital_new_press(DIGITAL_A))	endgame_s_p.toggleState();
	// 	if(master.get_digital_new_press(DIGITAL_A))	endgame_d_p.toggleState();
	// 	printf("%d %d\n", endgame_s_p.getState(), endgame_d_p.getState());
	// 	delay(10);
	// }


	driverPractice();
	
	// Auton::selectAuton();
	
	// Auton::runAuton();
	// spinRoller();
	// do{
	// 	roller_sensor.set_led_pwm(100);
  //   double cur_val = roller_sensor.get_rgb().red;
  //   printf("r: %lf \n", cur_val);
  //   _Task::delay(100);
  // }while(true);
	// autonLine();


	// autonStack();
	// driverPractice();

	WAIT_UNTIL(false);
	// indexer_p.setState(HIGH);
	// skills1();
/*
	while(true) {
		// printf("dist: %d \n", roller_sensor.get_proximity());
		if(master.get_digital_new_press(DIGITAL_A)){
			spinRoller();
			intake.waitToReachState(IntakeOffParams{});
		}
		// printf("r: %lf \n", roller_sensor.get_rgb().red);

		delay(10);
	}
*/
	// auton::program1();
	// cout << auton::arr[3]->name << endl;
	// cout << "size " << auton::arr.size() << endl;

	// cout << auton::GetCurAuton() << endl;


	// WAIT_UNTIL(false);


	// setFlywheelVel(2320);

	Timer disc_count_print{"disc_count_print"};
	Timer angle_override_print{"angle_override_print"};
	master.clear();
	drive.changeState(DriveOpControlParams{});
	while(true){

		// driveHandleInput();
		shooterHandleInput();
		intakeHandleInput();
		if((master.get_digital_new_press(DIGITAL_UP) || partner.get_digital_new_press(DIGITAL_UP)) && g_mag_disc_count < 3)	g_mag_disc_count++;
		if((master.get_digital_new_press(DIGITAL_DOWN) || partner.get_digital_new_press(DIGITAL_DOWN)) && g_mag_disc_count > 0)	g_mag_disc_count--; 


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
	

}
