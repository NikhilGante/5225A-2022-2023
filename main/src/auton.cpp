#include "auton.hpp"
#include "tracking.hpp"
#include "config.hpp"
#include "Subsystems/intake.hpp"
#include "Subsystems/flywheel.hpp"
#include "Subsystems/shooter.hpp"
#include "util.hpp"
// #include <sys/wait.h>

void moveInches(double target, double max_power, E_Brake_Modes brake_mode, double end_error){
	Timer move_timer{"move_timer"};
	double start = right_tracker.get_position()*1/36000.0 *(2.75*M_PI);
	double error;
  	log("Starting move %lf inches\n", target);
	int safety_count = 0;
	uint32_t cycle_timer = millis();
	do{
		double cur_y = right_tracker.get_position()*1/36000.0 *(2.75*M_PI) - start;
		error = target - cur_y;
		double power = 5.0*error;
		if(fabs(power) < 30) power = sgn(error) * 30;
		if(fabs(power) > max_power) power = sgn(error) * max_power;

		// if(fabs(power) > 100) power = sgn(error) * 100;
		moveDrive(power, 0.0);

		if(fabs(tracking.r_vel) < 0.1){
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

		_Task::delayUntil(cycle_timer, 10, "Move_Inches");

	}while(fabs(error) > end_error);
	// master.print(2, 0, "time: %ld", move_timer.getTime());
  log("Move %lf inches done, took %lld ms\n", target, move_timer.getTime());
	handleBrake(brake_mode);

  log("CONTROLLER RUMBLING FROM LINE 31 in file auton.cpp");
}

void autonStack(){
  Timer auton_timer{"Auton_timer"};

	setFlywheelVel(2420);
	tracking.reset({34, 12.25, 0.0});
	intakeOn();
	moveDrive(-50, 0);
	delay(250);
	intakeOff();
	moveInches(6);
	aimAtBlue(1);
	shootSync(2);
	intakeOn();
	setFlywheelVel(2335);

	turnToTargetSync({66, 40});
	moveToTargetSync({66, 40}, E_Brake_Modes::brake, 70);

	aimAtBlue(1);
	shootSync(3);


	delay(100);
	angler_p.toggleState();
	delay(250);
	angler_p.toggleState();


	master.printScroll("Time: %lld", auton_timer.getTime());
}

void autonAWP(){
	int temp = millis();
	Timer auton_timer{"Auton_timer"};
	setFlywheelVel(2210);


	tracking.reset({34, 12.25, 0.0});

	intakeOn();
	moveDrive(-30, 0);
	delay(250);
	intakeOff();

	moveInches(12, 127, E_Brake_Modes::brake, 4);
	
	turnToTargetSync({68.0, 51.0}, 0.5); // Go to centre field
	moveToTargetSync({68.0, 51.0}); // Go to centre field

	aimAtBlue(1);
	

	shootSync(2);
  
	setFlywheelVel(2320);
	moveInches(-12, 127, E_Brake_Modes::brake, 8);  // backup

	turnToTargetSync({100.0, 76.0}); // Face line



	moveToTargetSync({103.0, 79.0}, E_Brake_Modes::brake, 127, 2.0); // Move to corner

  	aimAtBlue(0.5);
  	// intake.waitToReachState(IntakeOffParams{});
	shootSync(3);
	
	turnToTargetSync({122.0, 108.0}, 0, true); // Face roller
	moveToTargetAsync({122.0, 108.0}, E_Brake_Modes::brake); // move to roller

	delay(100);
	angler_p.toggleState();
	delay(150);
	angler_p.toggleState();


	tracking.waitForComplete();

	intakeOff();
	turnToAngleSync(-90);

	moveDrive(-50, 0);
	delay(300);
	intakeOn();
	log("INIT VEL OF ROLLER: %f", tracking.r_vel);
	WAIT_UNTIL(fabs(tracking.r_vel) < 2);
	intakeOff();
	moveInches(8, 127, E_Brake_Modes::brake, 6);


	master.printScroll("Time: %d  ", millis()-temp);
	master.print(0, 0, "Time: %lld  ", auton_timer.getTime());

}

void autonLine(){
  	Timer auton_timer{"Auton_timer"};
	tracking.reset({126.5, 82.25, degToRad(-90)});
	setFlywheelVel(2345);
	intakeOn();

	moveToTargetSync({106, 82.25});
	master.printScroll("Time: %lld", auton_timer.getTime());

	aimAtBlue(0.5);

	shootSync(3);
	intakeOn();
	setFlywheelVel(2300);
	
	// WAIT_UNTIL(false);

	// turnToTargetSync({93, 93});
	// moveInches(13, 127, E_Brake_Modes::brake, 4);
	// moveInches(-13, 127, E_Brake_Modes::brake, 4);
	// move to disc and away


	turnToTargetSync({82, 59});
	master.printScroll("After turn: %lld", auton_timer.getTime());
	moveToTargetSync({82, 59});
	master.printScroll("After move: %lld", auton_timer.getTime());


	aimAtBlue(0.5);
	master.printScroll("After turn: %lld", auton_timer.getTime());
	shootSync(3);
	intakeOff();

	turnToTargetSync({122, 108}, 0.0, true);
	moveToTargetAsync({122, 108}, E_Brake_Modes::brake, 127, 1, E_Robot_Sides::back);

	delay(100);
	angler_p.toggleState();
	delay(150);
	angler_p.toggleState();

	tracking.waitForComplete();

	turnToAngleSync(-90);

	moveDrive(-40, 0);
	delay(250);
	WAIT_UNTIL(fabs(tracking.r_vel) < 2);
	intakeOn();
	delay(250);
	intakeOff();
	moveDrive(0, 0);
	moveInches(5, 127, E_Brake_Modes::brake, 3);


	master.printScroll("Final: %lld", auton_timer.getTime()); // End

}
void autonAWP2(){
	int temp = millis();
	Timer auton_timer{"Auton_timer"};
	setFlywheelVel(2210);


	tracking.reset({34, 12.25, 0.0});

	intakeOn();
	moveDrive(-30, 0);
	delay(250);
	intakeOff();

	moveInches(12, 127, E_Brake_Modes::brake, 4);
	
	turnToTargetSync({68.0, 51.0}, 0.5); // Go to centre field
	moveToTargetSync({68.0, 51.0}); // Go to centre field

	aimAtBlue(1);
	

	shootSync(2);
  
	setFlywheelVel(2340);
	moveInches(-12, 127, E_Brake_Modes::brake, 8);  // backup

	turnToTargetSync({100.0, 77.0}); // Face line



	moveToTargetSync({100.0, 77.0}, E_Brake_Modes::brake, 127, 2.0); // Move to corner

  	aimAtBlue(0.5);
  	// intake.waitToReachState(IntakeOffParams{});
	shootSync(3);
	
	turnToTargetSync({121.0, 105.0}, 0, true); // Face roller
	moveToTargetAsync({121.0, 105.0}, E_Brake_Modes::brake, 127, 2.0); // move to roller


	delay(100);
	angler_p.toggleState();
	delay(150);
	angler_p.toggleState();


	tracking.waitForDistance(25.0);
	intakeOff();
	moveToTargetAsync({135.0, 106.0}, E_Brake_Modes::brake, 127, 2.0); // move to roller

	// tracking.waitForDistance(12.0);
	// 

	WAIT_UNTIL(fabs(tracking.r_vel) < 10);
	drive.changeState(DriveIdleParams{});
	drive.waitToReachState(DriveIdleParams{});




	moveDrive(-30, 0);
	intakeOn();
	delay(300);
	// log("INIT VEL OF ROLLER: %f", tracking.r_vel);
	// WAIT_UNTIL(fabs(tracking.r_vel) < 2);
	intakeOff();
	moveInches(8, 127, E_Brake_Modes::brake, 6);


	master.printScroll("Time: %d  ", millis()-temp);
	master.print(0, 0, "Time: %lld  ", auton_timer.getTime());

}

void worldsSkills(){
	Timer auton_timer{"Auton_timer"};
	tracking.reset({34, 12.25, 0.0});
	setFlywheelVel(2420);

	moveDrive(-40, 0);
	intakeOn();
	delay(500);

	moveInches(3);

	aimAtBlue(2);

	shootSync(2);

	turnToAngleSync(0);
	setFlywheelVel(2200);



	moveForwardSync(29, E_Brake_Modes::brake, 65);
	aimAtBlue(2);
	WAIT_UNTIL(g_mag_disc_count >= 3);
	delay(100);
	shootSync(2);

	setFlywheelVel(1920);

	turnToTargetSync({36, 60});
	moveToTargetSync({36, 60});

	turnToTargetSync({60, 84});
	moveToTargetSync({60, 84});

	aimAtBlue(2);
	shootSync(3);


	turnToTargetSync({96, 120});
	setFlywheelVel(2190);
	moveToTargetSync({96, 120}, E_Brake_Modes::brake, 70);

	aimAtBlue(2);
	shootSync(3);

	turnToTargetSync({112, 122}, 0, true);
	moveToTargetSync({112, 122}, E_Brake_Modes::brake, 70, 1, E_Robot_Sides::back);

	turnToAngleSync(180);

	moveDrive(-30, 0);
	delay(300);
	WAIT_UNTIL(fabs(tracking.r_vel) < 2);
	delay(200);
	intakeOff();
	moveDrive(0, 0);

	moveInches(2);

	turnToTargetSync({125, 115});
	moveToTargetSync({125, 115});

	turnToAngleSync(-90);
	intakeOn();

	moveDrive(-30, 0);
	delay(300);
	WAIT_UNTIL(fabs(tracking.r_vel) < 2);
	delay(200);
	intakeOff();
	moveDrive(0, 0);

	moveInches(2);



	turnToTargetSync({124, 118});
	moveToTargetSync({124, 118});

	turnToAngleSync(-135);

	// Fire endgame



	master.printScroll("Time: %lld", auton_timer.getTime());

}