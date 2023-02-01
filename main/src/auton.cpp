#include "auton.hpp"
#include "tracking.hpp"
#include "config.hpp"
#include "Subsystems/intake.hpp"
#include "Subsystems/flywheel.hpp"
#include "Subsystems/shooter.hpp"

constexpr double DRIVEBASE_WIDTH = 14.5;
constexpr double LEFT_DIST_OFFSET = 1.75;  // How far in the left sensor is from left edge
constexpr double RIGHT_DIST_OFFSET = 2.125;  // How far in the right sensor is from right edge

double getDistL(){
  return (l_reset_dist.get()/25.4) - LEFT_DIST_OFFSET + DRIVEBASE_WIDTH/2;
}

double getDistR(){
  return (r_reset_dist.get()/25.4) - RIGHT_DIST_OFFSET + DRIVEBASE_WIDTH/2;
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

void fullSkills(){
  skills1();
  skills2();
  skills3();
}


// start coord 
// tracking.g_pos = {30.75, 7.375, degToRad(0.0)};	// ACTUAL SKILLS
void skills1(){
  tracking.reset({getDistL(), 7.5, 0.0});
  Timer total{"total_timer"};
  setFlywheelVel(2175);
  spinRoller();
  intake.waitToReachState(IntakeOffParams{});
  tracking.reset({31.0, 7.5, degToRad(0.0)});
  master.rumble("-");
  // WAIT_UNTIL(master.get_digital_new_press(DIGITAL_A));
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

	log("total: %d", total.getTime());
	lcd::print(7, "total: %d", total.getTime());
}

// tracking.g_pos = {110.5, 133.75, degToRad(180.0)}
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

	log("total: %d\n", total.getTime());
	master.print(0,0, "total: %d", total.getTime());
  // shoot match loads here
}
// tracking.g_pos = {72.0, 11.25, 0.0};
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


  log("total: %lld\n", total.getTime());
	lcd::print(7, "total: %lld", total.getTime());


  // expand
}

void autonStack(){
  // tracking.reset({30.75, 9.0, degToRad(0.0)});
  printf("STARTED AUTON\n");

  Timer timer1{"timer"};
  setFlywheelVel(2440);
	angler_p.setState(LOW);

  spinRoller();
  intake.waitToReachState(IntakeOffParams{});
  // tracking.reset({31.0, 7.375, degToRad(0.0)});
	// moveToTargetSync({37.0, 12.5});  // Move away from wall
  moveInches(5.0);
	aimAtBlue(10.0);
  printf("DONE AIMING: %lld %d\n", timer1.getTime(), millis());
	shoot(2);
  shooter.waitToReachState(ShooterIdleParams{});

	setFlywheelVel(2300);
	turnToTargetSync({69.0, 43.0});
	intakeOn();
	moveToTargetSync({69.0, 43.0}); // Pickup stack of discs
	aimAtBlue(11.0);
	shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});

	lcd::print(6, "total:%ld", timer1.getTime());

}

void autonAWP(){
  // tracking.reset({30.75, 9.0, degToRad(0.0)});

  Timer timer1{"timer"};
  setFlywheelVel(2300);
	angler_p.setState(LOW);

  moveDrive(-40, 0);
  delay(150);
  spinRoller(false);
  intake.waitToReachState(IntakeOffParams{});
  // tracking.reset({getDistL(), 7.375, degToRad(0.0)});
	// moveToTargetSync({30.75, 12.5});  // Move away from wall

  // moveInches(5.0);
  // moveInches(1.0);// Move away from wall


// YOOOO
	// turnToTargetSync({69.0, 43.0}, 0.0, false, E_Brake_Modes::brake, 2.0, 45); 
	intakeOn();
	moveToTargetSync({73.0, 45.0}, E_Brake_Modes::brake, 60); // Pickup stack of discs
	aimAtBlue(11.0);
  driveBrake();
  // WAIT_UNTIL(master.get_digital_new_press(DIGITAL_A));
	shoot(3);
  WAIT_UNTIL(false);
  shooter.waitToReachState(ShooterIdleParams{});
  intakeOn();
  setFlywheelVel(2440);
  
// YOOOO
	turnToTargetSync({122.0, 109.0}, 0.0, false, E_Brake_Modes::brake, 2.0, 100);
	// turnToTargetSync({124.0, 117.0}, 0.0, false, E_Brake_Modes::brake, 45);
	moveToTargetSync({122.0, 109.0},  E_Brake_Modes::coast, 110); // Move to corner

	turnToAngleSync(-90.0, E_Brake_Modes::brake, 2.0, 100);
  // spinRoller(false);


  
  spinRoller();
  intake.waitToReachState(IntakeOffParams{});

  // tracking.reset({141.0 - getDistR(), 141 - 9.75, degToRad(-90.0)});

  // moveInches(4.0);

  // aimAtBlue(12.0);
  // shoot(3);
  // shooter.waitToReachState(ShooterIdleParams{});
  master.print(2,0, "total:%ld", timer1.getTime());
	lcd::print(6, "total:%ld", timer1.getTime());
}

void autonLine(){
  tracking.reset({128.75, 83.25, degToRad(0.0)});
  
  Timer timer1{"timer"};
  setFlywheelVel(2420);
  moveToTargetSync({127, 108}, E_Brake_Modes::brake, 127); // move in front of roller
  turnToAngleSync(-90.0, E_Brake_Modes::brake, 2.0, 127);
  moveInches(-3.0);
  spinRoller();
  intake.waitToReachState(IntakeOffParams{});
  tracking.reset({133.75, 141-getDistR(), -M_PI_2});
  moveToTargetSync({128.0, tracking.g_pos.y});
  aimAtBlue(11);
  // turnToTargetSync(b_goal, 11, false, E_Brake_Modes::brake, 2.0, 70);
  aimAtBlue(11);
  driveBrake();
  shoot(2);

  shooter.waitToReachState(ShooterIdleParams{});

  setFlywheelVel(2270);
  turnToTargetSync({83.0, 61.0}, 0.0, false, E_Brake_Modes::brake, 2.0, 50);
  intakeOn();
  moveToTargetSync({83.0, 61.0},  E_Brake_Modes::brake, 100); // Drives through line
  aimAtBlue(11.5);
  // turnToTargetSync(b_goal, 11.5, false, E_Brake_Modes::brake, 2.0, 60);

  driveBrake();
  shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});

  master.print(2,0, "total:%ld", timer1.getTime());
	lcd::print(6, "total:%ld", timer1.getTime());
}