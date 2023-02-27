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
	// gyro.reset(true);
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

	drive.runMachine();
	intake.runMachine();
	flywheel.runMachine();
	shooter.runMachine();
	
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
	WAIT_UNTIL(!gyro.is_calibrating());
	autonAWP();
	// Auton::runAuton();
	// provSkills();
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


/*
Things to get done:
	- Test Flywheel Angle(Has to be good for close shots, skills, matchLoads, and autons)
	- Test flywheel drop after shot - Test for compression
	- Test toaster shots
	- Test flywheel Overheat
	- Test flywheel curve and calculate against old curve(10-12 degrees was old curve)
	- Test how fast we can shoot triple shot with flywheel
	- Test at what angles we can intake at and the range
	- 
Programmer Things:
	- Tune motions(Angle and drive to target)
	- Test tracking wheels with imu
		* Test for resets
		* Test for accuracy
		* Test for corkscrew and how much it affects along with movement of 1-3mm of the wheels
	- 
*/

/*
IMU THINGS:
	* A:
		- 
	* B:
		- 
	* C:
		- Multiply by 1.0108
		- Original Sensor
		- Doesn't drift much
*/

// 4.58, 4.53, 4.5, 4.57

// struct driveVar {
// 	std::string name;
// 	double value;

// 	driveVar(std::string n, double v): name(n), value(v){};
// };

void opcontrol() {

	// driverPractice();


	WAIT_UNTIL(!gyro.is_calibrating());

	double angle = atan((ultra_left.get_value()-ultra_right.get_value())/(12*25.4));
	tracking.reset({cos(degToRad(angle))*getDistL(), cos(degToRad(angle))*getDistBack(), angle});
	// double angle;
	// while (true) {
	// 	angle = radToDeg(atan((ultra_left.get_value()-ultra_right.get_value())/(12*25.4)));
	// 	cout << "ANGLE OF ROBOT: " << angle << endl;
	// 	cout << "X: " << cos(degToRad(angle))*getDistR() << endl;
	// 	cout << "Y: " << cos(degToRad(angle))*getDistBack() << endl;


	// 	delay(10);
	// }
	
	// autonLine();
	// flattenAgainstWallSync();
  // tracking.reset({getDistR(), getDistBack(), degToRad(0.0)});
	// spinRoller();
	// drive.changeState(DriveIdleParams{});
  // drive.waitToReachState(DriveIdleParams{});

	WAIT_UNTIL(false);
	// driverPractice();
	// trans_p.setState(HIGH);
	delay(100);
	moveDrive(-40, 0);
	delay(500);
	moveDrive(-10, 0);
	WAIT_UNTIL(false);
	Imu gyro_1(1);
	Imu gyro_2(2);
	Imu gyro_3(3);

// */
	
	// delay(3000);
	// while (true){

	// 	lcd::print(0, "Gyro_1: %.3f", gyro_1.get_rotation()*1.0108); // C - *1.0108
	// 	lcd::print(1, "Gyro_2: %.3f", gyro_2.get_rotation()*1.0027); // B - *1.0027
	// 	lcd::print(2, "Gyro_3: %.3f", gyro_3.get_rotation()*1.0125); // A - *1.0125
	// 	delay(50);
	// }


}
