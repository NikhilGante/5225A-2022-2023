#include "auton.hpp"
#include "tracking.hpp"
#include "Subsystems/intake.hpp"
#include "Subsystems/flywheel.hpp"
#include "Subsystems/shooter.hpp"

constexpr double DRIVEBASE_WIDTH = 14.5;
constexpr double LEFT_DIST_OFFSET = 2.25;  // How far in the left sensor is from left edge
constexpr double RIGHT_DIST_OFFSET = 2.5;  // How far in the right sensor is from right edge

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
  aimAtBlue(13);
  shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});

  turnToTargetSync({108, 126.0});
  intakeOn();
  moveToTargetSync({108, 126.0}, E_Brake_Modes::brake, 127);  // Drive to corner
  turnToAngleSync(177);

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

  moveToTargetSync({111.5, 120.75});
  aimAtRed(9);
  driveBrake();
  shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});

  setFlywheelVel(2100);
  intakeOn();
  moveToTargetSync({110.5, 90.0}, E_Brake_Modes::brake, 50); // Intake stack
  intake.waitToReachState(IntakeOffParams{}); // wait to intake 3rd disc
  aimAtRed(11);
  driveBrake();
  shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});

  setFlywheelVel(1950);
  intakeOn();
  moveToTargetSync({84.0, 60.0}); // Intake line
  aimAtRed(12);
  driveBrake();
  shoot(3); // Shoot from barrier
  shooter.waitToReachState(ShooterIdleParams{});

  moveToTargetSync({70.0, 70.0}); // Go to centre of field
  turnToTargetSync({26.0, 26.0}); // Turn to face corner
  intakeOn();
  moveToTargetSync({25.0, 25.0}, E_Brake_Modes::brake, 127); // Move to corner
  moveToTargetSync({28.0, 28.0}); // backup from corner
  turnToAngleSync(95);
  moveToTargetSync({9.0, 28.0}, E_Brake_Modes::coast, 127); // backup to wall
  // spinRoller();
  // intake.waitToReachState(IntakeOffParams{});
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


  moveToTargetSync({42.0, 30.0}); // Moves in front of stack
  aimAtRed(13);
  driveBrake();
  shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});

  setFlywheelVel(1975);
  intakeOn();
  moveToTargetSync({82.0, 57.0}); // Moves in front of barrier
  aimAtRed(13);
  driveBrake();
  shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});

  turnToTargetSync({130,111}, 0.0, true); // Face corner backwards
  moveToTargetSync({130,111}, E_Brake_Modes::brake, 127); // Move to corner

  turnToAngleSync(-90);
  moveToTargetSync({134,111}, E_Brake_Modes::coast, 127);
  spinRoller();
  intake.waitToReachState(IntakeOffParams{});
  moveInches(2.0);

  log("total: %lld\n", total.getTime());
	lcd::print(7, "total: %lld", total.getTime());
  // expand
}

void autonStack(){
  tracking.reset({30.75, 9.0, degToRad(0.0)});

  Timer timer1{"timer"};
  setFlywheelVel(2440);
	angler_p.setState(LOW);

  spinRoller();
  intake.waitToReachState(IntakeOffParams{});
  tracking.reset({31.0, 7.375, degToRad(0.0)});
	moveToTargetSync({30.75, 12.5});  // Move away from wall
	aimAtBlue(9.5);
	shoot(2);
  shooter.waitToReachState(ShooterIdleParams{});

	setFlywheelVel(2300);
	turnToTargetSync({69.0, 45.0});
	intakeOn();
	moveToTargetSync({69.0, 45.0}); // Pickup stack of discs
	aimAtBlue(9.0);
	shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});

	lcd::print(6, "total:%ld", timer1.getTime());

}

void autonAWP(){
  tracking.reset({30.75, 9.0, degToRad(0.0)});

  Timer timer1{"timer"};
  setFlywheelVel(2300);
	angler_p.setState(LOW);

  spinRoller();
  intake.waitToReachState(IntakeOffParams{});
  tracking.reset({31.0, 7.375, degToRad(0.0)});
	moveToTargetSync({30.75, 12.5});  // Move away from wall
	// aimAtBlue(9.5);
	// shoot(2);
  // shooter.waitToReachState(ShooterIdleParams{});

	turnToTargetSync({69.0, 45.0});
	intakeOn();
	moveToTargetSync({69.0, 45.0}); // Pickup stack of discs
	aimAtBlue(9.5);
	shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});
  intakeOn();
	turnToTargetSync({127.0, 114.0});
	moveToTargetSync({127.0, 114.0},  E_Brake_Modes::coast, 127); // Move to corner

	turnToAngleSync(-90.0);
  spinRoller();
  intake.waitToReachState(IntakeOffParams{});
	moveInches(2.0);  // Move away from wall
  master.print(2,0, "total:%ld", timer1.getTime());
	lcd::print(6, "total:%ld", timer1.getTime());
}

void autonLine(){
  tracking.reset({128.75, 83.25, degToRad(0.0)});
  
  Timer timer1{"timer"};
  setFlywheelVel(2420);
  moveToTargetSync({127, 110});
  turnToAngleSync(-90.0);
  moveInches(-2.0);
  spinRoller();
  intake.waitToReachState(IntakeOffParams{});
  tracking.reset({133.75, tracking.g_pos.y, -M_PI_2});
  moveToTargetSync({128.0, 112.25});
  aimAtBlue(11);
  driveBrake();
  shoot(2);

  shooter.waitToReachState(ShooterIdleParams{});

  setFlywheelVel(2270);
  turnToTargetSync({83.0, 59.0});
  intakeOn();
  moveToTargetSync({83.0, 59.0});
  aimAtBlue(11);
  driveBrake();
  shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});

  master.print(2,0, "total:%ld", timer1.getTime());
	lcd::print(6, "total:%ld", timer1.getTime());
}