#include "auton.hpp"
#include "tracking.hpp"
#include "config.hpp"
#include "Subsystems/intake.hpp"
#include "Subsystems/flywheel.hpp"
#include "Subsystems/shooter.hpp"
#include "util.hpp"
// #include <sys/wait.h>

void moveInches(double target, double max_power, E_Brake_Modes brake_mode){
	Timer move_timer{"move_timer"};
	double start = right_tracker.get_position()*1/36000.0 *(2.75*M_PI);
	double error;
  	log("Starting move %d inches\n", target);
	int safety_count = 0;
	do{
		double cur_y = right_tracker.get_position()*1/36000.0 *(2.75*M_PI) - start;
		error = target - cur_y;
		double power = 5.0*error;
		if(fabs(power) < 30) power = sgn(error) * 30;
		if(fabs(power) > max_power) power = sgn(error) * max_power;

		// if(fabs(power) > 100) power = sgn(error) * 100;
		moveDrive(power, 0.0);

		if(fabs(tracking.r_vel) < 0.5){
			safety_count++;
			if(safety_count > 20){
				moveDrive(0, 0);
				master.rumble(".");
				log("MOVE INCHES MOTION SAFETY TRIGGERED took %lld ms | Targ Inches:%lf | At x:%lf y:%lf, a:%lf\n", move_timer.getTime(), target, tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a));
				drive.changeState(DriveIdleParams{});
				return;
		}
		}
		else  safety_count = 0;

	}while(fabs(error) > 0.5);
	// master.print(2, 0, "time: %ld", move_timer.getTime());
  log("Move %lf inches done, took %lld ms\n", target, move_timer.getTime());
	handleBrake(brake_mode);

  log("CONTROLLER RUMBLING FROM LINE 31 in file auton.cpp");
}

void autonStack(){
  Timer auton_timer{"Auton_timer"};

	setFlywheelVel(2350);
	tracking.reset({34, 12.25, 0.0});
	intakeOn();
	moveDrive(-50, 0);
	delay(250);
	intakeOff();
	moveInches(6);
	aimAtBlue(2.5);
	shootSync(2);
	intakeOn();

	turnToTargetSync({66, 40});
	moveToTargetSync({66, 40}, E_Brake_Modes::brake, 70);

	aimAtBlue(2.5);
	shootSync(3);

	master.printScroll("Time: %d", auton_timer.getTime());
}

void autonAWP(){

	Timer auton_timer{"Auton_timer"};
	setFlywheelVel(2190);


	tracking.reset({34, 12.25, 0.0}); // HAS TO ADJUST RESET

	intakeOn();
	moveDrive(-30, 0);
	delay(250);
	intakeOff();

	moveToTargetSync({69.0, 55.0}); // Go to centre field

	aimAtBlue(1);
	shootSync(2);
  
	setFlywheelVel(2280);
	moveInches(-4);  // backup

	turnToTargetSync({102.0, 78.0}); // Face line

	moveToTargetSync({102.0, 78.0}, E_Brake_Modes::brake, 127, 2.0); // Move to corner

  	aimAtBlue(0.5);
  	// intake.waitToReachState(IntakeOffParams{});
	shootSync(3);
	
	turnToTargetSync({128.0, 103.0}, -20.0, true); // Face roller
	moveToTargetAsync({128.0, 103.0}, E_Brake_Modes::coast); // move to roller

	// DO SKETCHY THING TO BREAK ONE SIDE OF DRIVEBASE FOR QUICK TURN AND THEN MOVE BACKWARDS

	master.print(2,0, "total:%ld", auton_timer.getTime());

}

void autonLine(){ // No moving after start
  Timer auton_timer{"Auton_timer"};
	tracking.reset({126.5, 82.25, degToRad(-90)});
	setFlywheelVel(2350);
	intakeOn();

	moveToTargetSync({106, 82.25});
	master.printScroll("Time: %lld", auton_timer.getTime());

	aimAtBlue(2.5);

	shootSync(3);
	intakeOn();


	turnToTargetSync({82, 59});
	master.printScroll("After turn: %lld", auton_timer.getTime());
	moveToTargetSync({82, 59});
	master.printScroll("After move: %lld", auton_timer.getTime());


	aimAtBlue(2.5);
	master.printScroll("After turn: %lld", auton_timer.getTime());
	shootSync(2);
	intakeOff();

	turnToTargetSync({123, 111}, 0.0, true);
	moveToTargetSync({123, 111}, E_Brake_Modes::brake, 127, 1, E_Robot_Sides::back);

	turnToAngleSync(-90);

	moveDrive(-40, 0);
	delay(250);
	WAIT_UNTIL(tracking.r_vel < 2);
	intakeOn();
	delay(250);
	intakeOff();
	moveDrive(0, 0);
	moveInches(2);


	master.printScroll("Final: %lld", auton_timer.getTime()); // End

}