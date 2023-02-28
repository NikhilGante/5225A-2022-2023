#include "auton.hpp"
#include "Devices/controller.hpp"
#include "Devices/piston.hpp"
#include "Devices/others.hpp"
#include "Libraries/timer.hpp"
#include "Libraries/logging.hpp"
#include "Subsystems/intake.hpp"
#include "Subsystems/shooter.hpp"
#include "config.hpp"
#include "drive.hpp"
#include "tracking.hpp"
#include "util.hpp"

#include <numbers>

void moveInches(double target, double max_power){
	Timer move_timer{"Move Inches", auton_log};
	double start = right_tracker.getPos()*TICKS_TO_INCHES;
	double error;
	do{
		double cur_y = right_tracker.getPos()*TICKS_TO_INCHES - start;
		error = target - cur_y;
		double power = 5.0*error;
		if(std::abs(power) < 30) power = sgn(error) * 30;
		if(std::abs(power) > max_power) power = sgn(error) * max_power;

		// if(std::abs(power) > 100) power = sgn(error) * 100;
		moveDrive(power, 0.0);

	} while(std::abs(error) > 0.5);
	master.print(2, "time: %ld", move_timer.getTime());
	driveBrake();
	master.rumble();
  auton_log("CONTROLLER RUMBLING FROM LINE %d in file %s", __LINE__, __FILE__);
}

void fullSkills(){
  autonAWP();
  aimAtBlue(11.0);
  shoot(3);
  moveInches(15);
  turnToAngleSync(-135);
  endgame_s_p.setState(HIGH);
  endgame_d_p.setState(HIGH);

  // skills1();
  // skills2();
  // skills3();
}


// start coord 
// tracking.getPos() = {30.75, 7.375, degToRad(0.0)};	// ACTUAL SKILLS
void skills1(){
  tracking.reset({getDistL(), 7.5, 0.0});
  Timer total{"total_timer", auton_log};
  setFlywheelVel(2175);
  spinRoller();
  intake.waitToReachState(IntakeOffParams{});
  tracking.reset({31.0, 7.5, degToRad(0.0)});
  master.rumble();
  auton_log("CONTROLLER RUMBLING FROM LINE %d in file %s", __LINE__, __FILE__);
  // WAIT_UNTIL(master.getNewDigital(DIGITAL_A));
  intakeOn();
  moveToTargetSync({37.0, 43.0}, E_Brake_Modes::brake, 50); // picks up stack
  aimAtBlue(12.5);
  intake.waitToReachState(IntakeOffParams{}); // wait to intake 3rd disc
  driveBrake();
  shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});
  setFlywheelVel(1930);


  moveToTargetSync({62.0, 85.0}); // picks up line of discs
  aimAtBlue(13.5);
  shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});

  turnToTargetSync({113, 127.0});
  intakeOn();
  moveToTargetSync({113, 127.0});  // Drive to corner
  turnToAngleSync(177, E_Brake_Modes::brake, 2.0, 127);

	auton_log("total: %d", total.getTime());
	// auton_log("total: %d", total.getTime());
}

// tracking.getPos() = {110.5, 133.75, degToRad(180.0)}
void skills2(){
  Timer total{"total_timer", auton_log};

  setFlywheelVel(2340);
  spinRoller();
  intake.waitToReachState(IntakeOffParams{});
  tracking.reset({141.0 - getDistL(), 133.75, degToRad(180.0)});
  moveInches(2);
  turnToTargetSync({111.5, 120.75});
  moveToTargetSync({111.5, 120.75});

  // moveToTargetSync({111.5, 120.75});
  // moveInches(13);
  aimAtRed(9);
  driveBrake();
  shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});

  setFlywheelVel(2100);
  intakeOn();
  moveToTargetSync({110.5, 92.0}, E_Brake_Modes::brake, 50); // Intake stack
  // intake.waitToReachState(IntakeOffParams{}); // wait to intake 3rd disc
  delay(500);
  aimAtRed(11);
  driveBrake();
  shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});

  setFlywheelVel(1950);
  intakeOn();
  moveToTargetSync({84.0, 60.0}); // Intake line
  // turnToTargetSync(r_goal, 12.0, false, E_Brake_Modes::brake, 80);
  aimAtRed(12.0);

  driveBrake();
  shoot(3); // Shoot from barrier
  shooter.waitToReachState(ShooterIdleParams{});

  moveToTargetSync({71.0, 69.0}); // Go to centre of field
  turnToTargetSync({26.0, 26.0}); // Turn to face corner
  intakeOn();
  moveToTargetSync({25.0, 25.0}, E_Brake_Modes::brake, 127); // Move to corner
  moveToTargetSync({28.0, 28.0}); // backup from corner
  turnToAngleSync(95, E_Brake_Modes::brake, 2.0, 127);
  moveToTargetSync({9.0, 28.0}, E_Brake_Modes::coast, 127); // backup to wall

	auton_log("total: %d", total.getTime());
	master.print(0, "total: %d", total.getTime());
  // shoot match loads here
}
// tracking.getPos() = {72.0, 11.25, 0.0};
void skills3(){
  Timer total{"total_timer", auton_log};
  setFlywheelVel(2150);
  spinRoller();
  intake.waitToReachState(IntakeOffParams{});
  tracking.reset({7.25, getDistR(), degToRad(90.0)});
  moveInches(3.0);

  endgame_s_p.setState(HIGH);

/*

  moveToTargetSync({42.0, 30.0}); // Moves in front of stack
  aimAtRed(13);
  driveBrake();
  shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});

  setFlywheelVel(1975);
  intakeOn();
  // moveToTargetSync({82.0, 57.0}); // Moves in front of barrier

  // aimAtRed(13);
  // driveBrake();
  // shoot(3);
  // shooter.waitToReachState(ShooterIdleParams{});

  turnToTargetSync({10, 24}, 0.0, true, E_Brake_Modes::brake, 2.0, 127); // Face corner backwards
  moveToTargetSync({10, 24}, E_Brake_Modes::brake, 127); // Move to corner



  turnToTargetSync({130,111}, 0.0, false, E_Brake_Modes::brake, 2.0, 127); // Face corner backwards
  moveToTargetSync({130,111}, E_Brake_Modes::brake, 127); // Move to corner

  turnToAngleSync(-90, E_Brake_Modes::brake, 2.0, 127);
*/
  endgame_s_p.setState(HIGH);

  auton_log("total: %d", total.getTime());
	// auton_log(7, "total: %d", total.getTime());
  // expand
}

void autonStack(){
  // tracking.reset({30.75, 9.0, degToRad(0.0)});
  auton_log("STARTED AUTON\n");

  Timer timer1{"timer", auton_log};
  setFlywheelVel(2440);
	angler_p.setState(LOW);

  spinRoller();
  intake.waitToReachState(IntakeOffParams{});
  tracking.reset({getDistL(), 9.75, degToRad(0.0)});
  // tracking.reset({31.0, 7.375, degToRad(0.0)});
	// moveToTargetSync({37.0, 12.5});  // Move away from wall
  moveInches(5.0);
	aimAtBlue(10.0);
  auton_log("DONE AIMING: %lld %d", timer1.getTime(), millis());
	shoot(2);
  shooter.waitToReachState(ShooterIdleParams{});

	setFlywheelVel(2180);
  turnToTargetSync({73.0, 50.0}); // Faces stack
	intakeOn();
	moveToTargetSync({73.0, 50.0}, E_Brake_Modes::brake, 70); // Pickup stack of discs

	aimAtBlue(0);
  moveInches(7.0);
	aimAtBlue(11.0);


  driveBrake();
	shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});

	// turnToTargetSync({69.0, 43.0});
	// intakeOn();
	// moveToTargetSync({69.0, 43.0}); // Pickup stack of discs
	// aimAtBlue(11.0);
	// shoot(3);
  // shooter.waitToReachState(ShooterIdleParams{});
  master.print(2,0, "total:%ld", timer1.getTime());

	auton_log("total:%ld", timer1.getTime());

}

void autonAWP(){
  Timer timer1{"timer", auton_log};
  setFlywheelVel(2200);

  double angle = atan((ultra_left.getVal()-ultra_right.getVal())/(12*25.4));
	tracking.reset({cos(degToRad(angle))*getDistL(), cos(degToRad(angle))*getDistBack(), angle});


  // tracking.reset({getDistL(), 9.75, degToRad(0.0)});

  spinRoller();
  intake.waitToReachState(IntakeOffParams{});


  // WAIT_UNTIL(false);
	moveToTargetSync({tracking.getPos().x, 14.75}); // Moves away from wall

  // START of roller code

  // intakeRev();
  // drive.changeState(DriveIdleParams{});
  // drive.waitToReachState(DriveIdleParams{});
  
  // double intake_pos = intake_m.getPosition();

  // moveDrive(-40, 0);
  // WAIT_UNTIL(std::abs(intake_pos- intake_m.getPosition()) > 450);
  // intakeOff();
  // moveInches(2.0);


  // END of roller code


  // YOOOO
  turnToTargetSync({73.0, 48.0}); // Faces stack
	intakeOn();
	moveToTargetSync({73.0, 48.0}, E_Brake_Modes::brake, 70); // Pickup stack of discs

	aimAtBlue(0);

  driveBrake();
	shoot(3);
  // WAIT_UNTIL(false);
  // WAIT_UNTIL(master.getNewDigital(DIGITAL_A));
  shooter.waitToReachState(ShooterIdleParams{});
  intakeOn();
  setFlywheelVel(2420);
  
	turnToTargetSync({125.0, 115.0}); // Face corner
  
	// turnToTargetSync({124.0, 117.0}, 0.0, false, E_Brake_Modes::brake, 45);
	moveToTargetSync({125.0, 115.0}); // Move to corner
  auton_log("TURNED INTAKE OFF\n");
  intakeOff();

	turnToAngleSync(-90.0, E_Brake_Modes::brake, 3.5);

  
  spinRoller();
  intake.waitToReachState(IntakeOffParams{});

  moveInches(4.0);

  // aimAtBlue(0.0);
  // shoot(3);
  // WAIT_UNTIL(timer1.getTime() > 15000) master.rumble("---");
  auton_log("CONTROLLER RUMBLING FROM LINE %d in file %s", __LINE__, __FILE__);
  shooter.waitToReachState(ShooterIdleParams{});
  master.print(2,0, "total:%ld", timer1.getTime());
	auton_log("total:%ld", timer1.getTime());
}

void autonLine(){ // No moving after start

  tracking.reset({128.75, 83.25, degToRad(0.0)});

  Timer timer1{"timer", auton_log};
  setFlywheelVel(2255);
  moveToTargetSync({tracking.getPos().x, 108}, E_Brake_Modes::brake, 127); // move in front of roller
  turnToAngleSync(-90.0, E_Brake_Modes::brake, 2.0, 127);

  flattenAgainstWallSync();
	double angle = atan((ultra_left.getVal()-ultra_right.getVal())/(12*25.4));
	tracking.reset({141-cos(degToRad(angle))*getDistBack(), 141-cos(degToRad(angle))*getDistR(), angle-degToRad(90)});

  spinRoller();
  intake.waitToReachState(IntakeOffParams{});

  moveInches(5);
  intakeOn();
  turnToTargetSync({103.0, 79.0}); // Drives through line
  moveToTargetSync({105.0, 81.0},  E_Brake_Modes::brake, 127); // Drives through line
  aimAtBlue(3);
  driveBrake();
  shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});

  turnToTargetSync({83.0, 60.0}); // Drives through line
  setFlywheelVel(2185);
  moveToTargetSync({83.0, 60.0},  E_Brake_Modes::brake, 127); // Drives through line
  aimAtBlue(3);
  driveBrake();
  shoot(2);
  shooter.waitToReachState(ShooterIdleParams{});
  master.print(2,0, "total:%ld", timer1.getTime());


/*  Timer timer1{"timer", auton_log};
  angler_p.setState(LOW);

  flattenAgainstWallSync();
  tracking.reset({131.25, 141-getDistR(), degToRad(-90.0)});
  trans_p.setState(HIGH);

  setFlywheelVel(2215);
  intakeOn();
  moveToTargetSync({96, 82}); // move to single disc in front
  aimAtBlue(10.5);


  driveBrake();
  shoot(3);

  shooter.waitToReachState(ShooterIdleParams{});
  setFlywheelVel(2250);

  moveToTargetSync({102, 78}); // Backup
  intakeOn();
  turnToTargetSync({84, 60}, 0.0, false, E_Brake_Modes::brake, 3);
  moveToTargetSync({84, 60}); // Pickup line
  aimAtBlue(11);
  shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});


  turnToTargetSync({126, 108}, 0.0, true); // Turn to face corner
  intakeOff();
  moveToTargetSync({128, 108}, E_Brake_Modes::coast); // Backup to corner

  intakeRev();
  drive.changeState(DriveIdleParams{});
  drive.waitToReachState(DriveIdleParams{});
  
  double intake_pos = intake_m.get_position();

  moveDrive(-40, 0);
  WAIT_UNTIL(std::abs(intake_pos- intake_m.get_position()) > 450);
  intakeOff();
  moveInches(2.0);

  // intake.waitToReachState(IntakeOffParams{});
  
  master.print(2,0, "total:%ld", timer1.getTime());
	auton_log("total:%ld", timer1.getTime());
 
*/
}



void provSkills(){
  Timer timer1{"timer"};
  WAIT_UNTIL(master.getNewDigital(DIGITAL_A));
  intakeOn();
  moveInches(45, 60);
  WAIT_UNTIL(false);

  /*
  spinRoller();
  setFlywheelVel(2150);
  intake.waitToReachState(IntakeOffParams{});
  tracking.reset({getDistL(), 9.75, degToRad(0.0)});

  moveToTargetAsync({35, 45}, E_Brake_Modes::brake, 55);
  tracking.waitForDistance(10);
  intakeOn();
  tracking.waitForComplete();
  aimAtBlue(12);
  intake.waitToReachState(IntakeOffParams{});
  shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});
  
  intakeOn();
  setFlywheelVel(2000);
  moveInches(8);
  turnToTargetSync({57, 83});
  moveToTargetSync({57, 83}, E_Brake_Modes::brake, 85); // picks up line of discs
  aimAtBlue(10.5);
  driveBrake();
  shoot(3); // Shoot shots from barrier corner
  shooter.waitToReachState(ShooterIdleParams{});

  setFlywheelVel(1900);
  turnToAngleSync(-12.0);
  moveInches(45, 50);
  // moveToTargetSync({51, 141-12});

  aimAtBlue(13.5);
  driveBrake();
  shoot(3); // Shoots shots from inside barrier
  shooter.waitToReachState(ShooterIdleParams{});

  turnToTargetSync({102, 129}, 0.0, true);
  intakeOff();
  moveToTargetSync({102, 129});
  turnToAngleSync(-180);
  
  */
  spinRoller();
  setFlywheelVel(2150);
  intake.waitToReachState(IntakeOffParams{});
  // tracking.reset({getDistL(), 9.75, tracking.g_pos.a+M_PI});
  tracking.reset({getDistL(), 9.75, tracking.getPos().a});
  // intakeOn();
  
  // REPEATED STUFF

  moveToTargetAsync({35, 45}, E_Brake_Modes::brake, 55);
  delay(750);
  intakeOn();
  tracking.waitForComplete();

  aimAtBlue(11.5);
  intake.waitToReachState(IntakeOffParams{});
  shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});
  
  intakeOn();
  setFlywheelVel(2000);
  moveInches(8);
  turnToTargetSync({57, 83});
  moveToTargetSync({57, 83}, E_Brake_Modes::brake, 85); // picks up line of discs
  aimAtBlue(10);
  driveBrake();
  shoot(3); // Shoot shots from barrier corner
  shooter.waitToReachState(ShooterIdleParams{});

  moveInches(-15);
  turnToAngleSync(45);
  moveToTargetSync({120, 120});
  turnToAngleSync(-90);
  moveInches(-10);
  spinRoller();
  intake.waitToReachState(IntakeOffParams{});
  tracking.reset({141-9.75, 141-getDistR(), degToRad(-90)});
  moveToTargetSync({100, 116});

  

  master.print(2,0, "total:%ld", timer1.getTime());
	auton_log("total:%ld\n", timer1.getTime());
}