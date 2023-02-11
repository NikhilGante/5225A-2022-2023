#include "auton.hpp"
#include "Devices/controller.hpp"
#include "Devices/motor.hpp"
#include "Devices/piston.hpp"
#include "Libraries/timer.hpp"
#include "Subsystems/flywheel.hpp"
#include "Subsystems/intake.hpp"
#include "Subsystems/shooter.hpp"
#include "config.hpp"
#include "drive.hpp"
#include "tracking.hpp"
#include "util.hpp"

static constexpr double TICKS_TO_INCHES_275 = 2.75*std::numbers::pi/36000;
static constexpr double MM_TO_IN = 1/2.54/10;
static constexpr double HALF_DRIVEBASE_WIDTH = 14.5/2;
static constexpr double LEFT_DIST_OFFSET = 1.75;  // How far in the left sensor is from left edge
static constexpr double RIGHT_DIST_OFFSET = 2.125;  // How far in the right sensor is from right edge

double getDistL() {return l_reset_dist.get()*MM_TO_IN - LEFT_DIST_OFFSET  + HALF_DRIVEBASE_WIDTH;}
double getDistR() {return r_reset_dist.get()*MM_TO_IN - RIGHT_DIST_OFFSET + HALF_DRIVEBASE_WIDTH;}

void moveInches(double target){
	Timer move_timer{"move_timer"};
	double start = left_tracker.get_position()*TICKS_TO_INCHES_275;
	double error;
	do{
		double cur_y = left_tracker.get_position()*TICKS_TO_INCHES_275 - start;
		error = target - cur_y;
		double power = 5.0*error;
		if(std::abs(power) < 30) power = sgn(error) * 30;
		// if(std::abs(power) > 100) power = sgn(error) * 100;
		moveDrive(power, 0.0);

	} while(std::abs(error) > 0.5);
	master.print(2, "time: %ld", move_timer.getTime());
	driveBrake();
	master.rumble("-");
}

void fullSkills(){
  skills1();
  skills2();
  skills3();
}


// start coord 
// tracking.getPos() = {30.75, 7.375, degToRad(0.0)};	// ACTUAL SKILLS
void skills1(){
  tracking.reset({getDistL(), 7.5, 0.0});
  Timer total{"total_timer"};
  setFlywheelVel(2175);
  spinRoller();
  intake.waitToReachState(IntakeOffParams{});
  tracking.reset({31.0, 7.5, degToRad(0.0)});
  master.rumble("-");
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
  Timer total{"total_timer"};

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
  Timer total{"total_timer"};
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

  Timer timer1{"timer"};
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

	setFlywheelVel(2150);
  turnToTargetSync({73.0, 50.0}); // Faces stack
	intakeOn();
	moveToTargetSync({73.0, 50.0}, E_Brake_Modes::brake, 70); // Pickup stack of discs

	aimAtBlue(0);
  moveInches(10.0);
	aimAtBlue(11.5);


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


  Timer timer1{"timer"};
  setFlywheelVel(2300);

  spinRoller();
  intake.waitToReachState(IntakeOffParams{});
  tracking.reset({getDistL(), 9.75, degToRad(0.0)});

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

	aimAtBlue(11.5);

  driveBrake();
	shoot(3);
  // WAIT_UNTIL(false);
  // WAIT_UNTIL(master.getNewDigital(DIGITAL_A));
  shooter.waitToReachState(ShooterIdleParams{});
  intakeOn();
  setFlywheelVel(2420);
  
// YOOOO
	turnToTargetSync({125.0, 105.0}); // Face corner
  
	// turnToTargetSync({124.0, 117.0}, 0.0, false, E_Brake_Modes::brake, 45);
	moveToTargetSync({125.0, 105.0}); // Move to corner
  auton_log("TURNED INTAKE OFF\n");
  intakeOff();

	turnToAngleSync(-90.0, E_Brake_Modes::brake, 3.5);

  
  spinRoller();
  intake.waitToReachState(IntakeOffParams{});

  moveInches(4.0);

  // aimAtBlue(11.0);
  // shoot(3);
  WAIT_UNTIL(timer1.getTime() > 15000) master.rumble("---");
  shooter.waitToReachState(ShooterIdleParams{});
  master.print(2,0, "total:%ld", timer1.getTime());
	auton_log("total:%ld", timer1.getTime());
}

void autonLine(){ // No moving after start
  Timer timer1{"timer"};
  angler_p.setState(LOW);

  flattenAgainstWallSync();
  tracking.reset({131.25, 141-getDistR(), degToRad(-90.0)});

  setFlywheelVel(2215);
  intakeOn();
  moveToTargetSync({96, 82}); // move to single disc in front
  aimAtBlue(10.5);


  driveBrake();
  shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});
  setFlywheelVel(2225);

  moveToTargetSync({102, 78}); // Backup
  intakeOn();
  turnToTargetSync({84, 60}, 0.0, false, E_Brake_Modes::brake, 3); // Backup
  moveToTargetSync({84, 60}); // Backup
  aimAtBlue(10);
  shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});


  turnToTargetSync({126, 108}, 0.0, true, E_Brake_Modes::coast); // Backup
  intakeOff();
  moveToTargetSync({128, 108}); // Backup

  intakeRev();
  drive.changeState(DriveIdleParams{});
  drive.waitToReachState(DriveIdleParams{});
  
  double intake_pos = intake_m.getPosition();

  moveDrive(-40, 0);
  WAIT_UNTIL(std::abs(intake_pos- intake_m.getPosition()) > 450);
  intakeOff();
  moveInches(2.0);

  intake.waitToReachState(IntakeOffParams{});
  
  master.print(2,0, "total:%ld", timer1.getTime());
	auton_log("total:%ld", timer1.getTime());
}

void autonLineTestTurn(){ // Moving 90 degrees after start
  Timer timer1{"timer"};
  angler_p.setState(LOW);

  
  turnToAngleSync(-90);
  setFlywheelVel(2225);
  intakeOn();
  moveToTargetSync({96, 84}, E_Brake_Modes::brake, 70); // move to single disc in front
  aimAtBlue(11);
  driveBrake();
  shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});

  // Turn back to spin roller, than forward to get the last two discs



  driveBrake();
  master.print(2,0, "total:%ld", timer1.getTime());
	auton_log("total:%ld", timer1.getTime());
 

}


void autonLineOld(){
  // tracking.reset({128.75, 83.25, degToRad(0.0)});
  
  Timer timer1{"timer"};
  setFlywheelVel(2420);
  moveToTargetSync({tracking.getPos().x, 102}, E_Brake_Modes::brake, 127); // move in front of roller
  turnToAngleSync(-90.0, E_Brake_Modes::brake, 2.0, 127);
  moveInches(-3.0);
  spinRoller();
  intake.waitToReachState(IntakeOffParams{});
  tracking.reset({131.25, 141-getDistR(), degToRad(-90.0)});

  
  moveToTargetSync({128.0, tracking.getPos().y});
  aimAtBlue(10, 60, 3);
  moveInches(3);
  aimAtBlue(10, 60, 1);
  // turnToTargetSync(b_goal, 11, false, E_Brake_Modes::brake, 2.0, 70);
  // aimAtBlue(11);

  driveBrake();
  shoot(2);

  shooter.waitToReachState(ShooterIdleParams{});

  // setFlywheelVel(2200);
  // turnToTargetSync({83.0, 61.0}, 0.0, false, E_Brake_Modes::brake, 2.0, 80);
  // intakeOn();
  // moveToTargetSync({82.0, 58.0},  E_Brake_Modes::brake, 115); // Drives through line
  // aimAtBlue(13);
  // moveInches(8);
  // aimAtBlue(13);
  // // turnToTargetSync(b_goal, 11.5, false, E_Brake_Modes::brake, 2.0, 60);

  // driveBrake();
  // shoot(3);
  // shooter.waitToReachState(ShooterIdleParams{});

  master.print(2,0, "total:%ld", timer1.getTime());
	auton_log("total:%ld", timer1.getTime());
}

void autonLinePrev(){
  // tracking.reset({128.75, 83.25, degToRad(0.0)});
  
  Timer timer1{"timer"};
  setFlywheelVel(2300, 415);
  moveToTargetSync({tracking.getPos().x, 104}, E_Brake_Modes::brake, 127); // move in front of roller
  turnToAngleSync(-90.0, E_Brake_Modes::brake, 2.0, 127);
  moveInches(-3.0);
  spinRoller();
  intake.waitToReachState(IntakeOffParams{});
  tracking.reset({131.25, 141-getDistR(), degToRad(-90.0)});

  moveInches(5);
  intakeOn();
  turnToTargetSync({103.0, 79.0}); // Drives through line
  moveToTargetSync({105.0, 81.0},  E_Brake_Modes::brake, 127); // Drives through line
  aimAtBlue(10);
  driveBrake();
  shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});
  setFlywheelVel(2300, 423);

  turnToTargetSync({85.0, 60.0}); // Drives through line
  moveToTargetSync({85.0, 60.0},  E_Brake_Modes::brake, 127); // Drives through line
  aimAtBlue(12);
  driveBrake();
  shoot(2);


  
  // // turnToTargetSync(b_goal, 11.5, false, E_Brake_Modes::brake, 2.0, 60);

  // driveBrake();
  // shoot(3);
  // shooter.waitToReachState(ShooterIdleParams{});

  master.print(2,0, "total:%ld", timer1.getTime());
  master.print(2, "total:%ld", timer1.getTime());
	auton_log("total:%ld", timer1.getTime());
}



void insaneHighPointSkills(){
  

  Timer timer1{"timer"};
  setFlywheelVel(2200);

  spinRoller();
  intake.waitToReachState(IntakeOffParams{});
  tracking.reset({getDistL(), 9.75, degToRad(0.0)});

	moveToTargetSync({tracking.getPos().x, 14.75}); // Moves away from wall
  aimAtRed(8);
  shoot(2);

  turnToTargetSync({73.0, 48.0}); // Faces stack
	intakeOn();
	moveToTargetSync({73.0, 48.0}, E_Brake_Modes::brake, 70); // Pickup stack of discs

	aimAtRed(11.5);

  driveBrake();
	shoot(3);
  // WAIT_UNTIL(false);
  // WAIT_UNTIL(master.getNewDigital(DIGITAL_A));
  shooter.waitToReachState(ShooterIdleParams{});
  intakeOn();
  setFlywheelVel(2300);
  

	turnToTargetSync({125.0, 105.0}); // Face corner
  
	// turnToTargetSync({124.0, 117.0}, 0.0, false, E_Brake_Modes::brake, 45);
	moveToTargetSync({125.0, 105.0}); // Move to corner
  auton_log("TURNED INTAKE OFF\n");
  intakeOff();

	turnToAngleSync(-90.0, E_Brake_Modes::brake, 3.5);

  
  spinRoller();
  intake.waitToReachState(IntakeOffParams{});

  moveInches(8.0);

  aimAtRed(5.0);
  moveInches(15);
  aimAtRed(11);
  shoot(3);
  
  turnToTargetSync({120, 120});
  moveToTargetSync({120, 120});
  turnToAngleSync(-135);

  // FireEndgame

  // shooter.waitToReachState(ShooterIdleParams{});
  master.print(2,0, "total:%ld", timer1.getTime());
	auton_log("total:%ld", timer1.getTime());
 

}
