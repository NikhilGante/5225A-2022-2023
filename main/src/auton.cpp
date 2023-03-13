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

void moveInches(double target, double max_power, E_Brake_Modes brake_mode){
	Timer move_timer{"Move Inches", auton_log};
	double start = right_tracker.getVal()*TICKS_TO_INCHES;
	double error;
	do{
		double cur_y = right_tracker.getVal()*TICKS_TO_INCHES - start;
		error = target - cur_y;
		double power = 5.0*error;
		if(std::abs(power) < 30) power = sgn(error) * 30;
		if(std::abs(power) > max_power) power = sgn(error) * max_power;

		// if(std::abs(power) > 100) power = sgn(error) * 100;
		moveDrive(power, 0.0);

	} while(std::abs(error) > 0.5);
	master.print(2, "time: %ld", move_timer.getTime());
	handleBrake(brake_mode);

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
  Timer total{"Total", auton_log};
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

	total.print();
	// total.print();
}

// tracking.getPos() = {110.5, 133.75, degToRad(180.0)}
void skills2(){
  Timer total{"Total", auton_log};

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

	total.print();
	master.print(0, "total: %d", total.getTime());
  // shoot match loads here
}
// tracking.getPos() = {72.0, 11.25, 0.0};
void skills3(){
  Timer total{"Total", auton_log};
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

  total.print();
  // expand
}


/*
X: 72.3, Y: 49.72, A: -34.99
  - 1.25 Inches Backwards
*/


void autonStack(){
  WAIT_UNTIL(!gyro.isCalibrating());
  tracking.reset({30.75, 9.0, degToRad(0.0)});
  Timer timer1{"imer", auton_log};
  setFlywheelVel(2350);
	angler_p.setState(LOW);

  Position pos = distanceReset(resetPosition::leftHome);
  if(std::abs(pos.x - tracking.getPos().x) < 2 && std::abs(pos.y - tracking.getPos().y) < 2) tracking.reset(distanceReset(resetPosition::leftHome));

  spinRoller();
  intake.waitToReachState(IntakeOffParams{});

  // tracking.reset({31.0, 7.375, degToRad(0.0)});
	// moveToTargetSync({37.0, 12.5});  // Move away from wall
  moveInches(5.0);
	aimAtBlue(1.5);
  auton_log("DONE AIMING: %lld %d\n", timer1.getTime(), millis());
	shoot(2);
  shooter.waitToReachState(ShooterIdleParams{});

	setFlywheelVel(2180);
  turnToTargetSync({73.0, 50.0}); // Faces stack
	intakeOn();
	moveToTargetSync({73.0, 50.0}, E_Brake_Modes::brake, 70); // Pickup stack of discs

	aimAtBlue(1.5);

  // shoot(1);
  // shooter.waitToReachState(ShooterIdleParams{});
  // intakeOn();
  // moveInches(7.0);
	// aimAtBlue(1.5);


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

	timer1.print();

}

void autonAWP(){
  WAIT_UNTIL(!gyro.isCalibrating());
  Timer timer1{"imer", auton_log};
  setFlywheelVel(2270);

  tracking.reset(distanceReset(resetPosition::leftHome));



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
  turnToTargetSync({76.0, 44.0}); // Faces stack
	intakeOn();
	moveToTargetSync({76.0, 44.0}, E_Brake_Modes::brake, 80); // Pickup stack of discs
  delay(250);

	aimAtBlue(1);

  driveBrake();
	shoot(3);
  // WAIT_UNTIL(false);
  // WAIT_UNTIL(master.getNewDigital(DIGITAL_A));
  shooter.waitToReachState(ShooterIdleParams{});
  intakeOn();
  setFlywheelVel(2375);
  
  // YOOOO
	turnToTargetSync({130.0, 110.0}); // Face corner
  
	// turnToTargetSync({124.0, 117.0}, 0.0, false, E_Brake_Modes::brake, 45);
	moveToTargetSync({130.0, 110.0}); // Move to corner
  auton_log("TURNED INTAKE OFF\n");
  intakeOff();

	turnToAngleSync(-90.0, E_Brake_Modes::brake, 3.5);
  // flattenAgainstWallSync();

  // tracking.reset(distanceReset(resetPosition::rightAway, -90));
  
  spinRoller();
  intake.waitToReachState(IntakeOffParams{});

  moveInches(4.0);

  // aimAtBlue(0.5);
  // shoot(3);
  // WAIT_UNTIL(timer1.getTime() > 15000) master.rumble("---");
  // auton_log("CONTROLLER RUMBLING FROM LINE %d in file %s", __LINE__, __FILE__);
  shooter.waitToReachState(ShooterIdleParams{});
  master.print(2,0, "total:%ld", timer1.getTime());
	timer1.print();
}

void autonLine(){ // No moving after start
  WAIT_UNTIL(!gyro.isCalibrating());
  tracking.reset({129.5, 86.00, degToRad(0.0)});

  Timer timer1{"imer", auton_log};
  setFlywheelVel(2255);
  moveToTargetSync({tracking.getPos().x, 112}, E_Brake_Modes::brake, 127); // move in front of roller
  turnToAngleSync(-90.0, E_Brake_Modes::brake, 2.0, 127);

  flattenAgainstWallSync();
  Position pos = distanceReset(resetPosition::rightAway);
  if(std::abs(pos.x - tracking.getPos().x) < 3 && std::abs(pos.y - tracking.getPos().y) < 3)  tracking.reset(pos);
  else tracking.resetA(pos.a);
  // tracking.reset(distanceReset(resetPosition::rightAway));


  spinRoller();
  intake.waitToReachState(IntakeOffParams{});

  moveInches(5);
  intakeOn();
  turnToTargetSync({103.0, 79.0}); // Drives through line
  moveToTargetSync({105.0, 81.0},  E_Brake_Modes::brake, 127); // Drives through line
  aimAtBlue(1);
  driveBrake();
  shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});

  turnToTargetSync({83.0, 60.0}); // Drives through line
  setFlywheelVel(2175);
  moveToTargetSync({83.0, 60.0},  E_Brake_Modes::brake, 127); // Drives through line
  aimAtBlue(3);
  driveBrake();
  shoot(2);
  shooter.waitToReachState(ShooterIdleParams{});
  master.print(2,0, "total:%ld", timer1.getTime());


/*  Timer timer1{"imer", auton_log};
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
	timer1.print();
 
  */
}

void provSkills(){
	WAIT_UNTIL(!gyro.isCalibrating());
	Timer total_timer{"Total"};
// /*
	tracking.reset({74.1952, 8.29092, 1.40752});

	setFlywheelVel(barrier_rpm - 50);
	shoot(9, true);	
	shooter.waitToReachState(ShooterIdleParams{});
  shoot(3);	
	shooter.waitToReachState(ShooterIdleParams{});

	moveToTargetSync({tracking.getPos().x + 10, tracking.getPos().y + 3});
	turnToTargetSync({33, 17}, 0.0, true);
	moveToTargetSync({33, 17});

	turnToAngleSync(0);


// SKILLS START (from roller) ---------


	setFlywheelVel(2115);

	flattenAgainstWallSync();
	tracking.reset(distanceReset(resetPosition::leftHome));
  // delay(2000);
	spinRoller(600);
	intake.waitToReachState(IntakeOffParams{});
  moveInches(1);
	turnToTargetSync({35.0, 46.0});

	intakeOn();
	moveToTargetSync({35.0, 46.0}, E_Brake_Modes::brake, 55);
  delay(500); // Intakes last disc
	aimAtBlue(4);
	driveBrake();
	shoot(3);
	shooter.waitToReachState(ShooterIdleParams{});

	setFlywheelVel(1820);
	moveToTargetSync({68.0, 90.0}, E_Brake_Modes::brake, 100);	// Go to barrier
	aimAtBlue(3);
	shoot(3);
	shooter.waitToReachState(ShooterIdleParams{});


	turnToTargetSync({111, 127});	// Go to corner
	intakeOn();
	moveToTargetSync({111, 127}, E_Brake_Modes::brake, 90);
	turnToAngleSync(180);


// */..
  // Match loader routine
  setFlywheelVel(1750);

	flattenAgainstWallSync();
	tracking.reset(distanceReset(resetPosition::leftHome));

  spinRoller(600);
	intake.waitToReachState(IntakeOffParams{});

	turnToAngleSync(101);	// Face wall
	moveInches(35.0, 70, E_Brake_Modes::coast);
	// driveBrake();
	master.rumble("-");
	moveDrive(25, 0.0);
	delay(300);	// wait for speed up
	while(ultra_left.getVal()*MM_TO_IN < 67){
		auton_log("***Ultra: %lf\n", ultra_left.getVal()*MM_TO_IN);
		delay(10);
	}
	driveBrake();
	aimAtRed(2.0);

	driveBrake();
	shoot(10, true);
	shooter.waitToReachState(ShooterIdleParams{});

  // End of match loader routine

  // Returns to roller
  moveToTargetSync({tracking.getPos().x + 10, tracking.getPos().y + 3});
	turnToTargetSync({32, 15}, 0.0, true);
	moveToTargetSync({32, 15});

	turnToAngleSync(0);



// SECTION 2 -------------------------------------------


	setFlywheelVel(2045);
	flattenAgainstWallSync();
	tracking.reset(distanceReset(resetPosition::leftHome));
  moveDrive(0.0, 0.0);
  trans_p.setState(HIGH);
  delay(100); // Waits for transmission to shift

  intakeOn();
	turnToTargetSync({35.0, 23.0});
	moveToTargetSync({35.0, 23.0}, E_Brake_Modes::coast);
	
	// Goes to stack
	turnToTargetSync({35.0, 48.0});
	moveToTargetSync({35.0, 48.0});
	aimAtBlue(2);
	driveBrake();
	shoot(3);
	shooter.waitToReachState(ShooterIdleParams{});

	setFlywheelVel(1850);
	moveToTargetSync({60.0, 82.0}, E_Brake_Modes::brake, 100);	// Go to barrier
	aimAtBlue(2);
	shoot(3);
	shooter.waitToReachState(ShooterIdleParams{});

  turnToTargetSync({24.0, 24.0}, 0.0, true);  // Face corner
  moveToTargetSync({24.0, 24.0});  // Face corner
  turnToAngleSync(45.0);
  
  endgame_s_p.setState(HIGH);
  endgame_d_p.setState(HIGH);

	// moveToTargetSync({tracking.getPos().x + 10.0, tracking.getPos().y - 10.0});
	// turnToTargetSync({125.0, 125.0});
	// intakeOn();
	// moveToTargetSync({120.0, 120.0}, E_Brake_Modes::brake, 100);
	// moveToTargetSync({112.0, 112.0});

	// turnToAngleSync(-90);
	// flattenAgainstWallSync();
	// tracking.reset(distanceReset(resetPosition::rightHome, 90));
	// spinRoller(600);
  // intake.waitToReachState(IntakeOffParams{});

  // moveInches(1.0);
  // turnToTargetSync({24.0, 24.0});
  // moveToTargetSync({24.0, 24.0});
  // turnToAngleSync(45.0);

	master.print(2, 0, "total: %d", total_timer.getTime());

  WAIT_UNTIL(false);
}

void backupSkills(){
	WAIT_UNTIL(!gyro.isCalibrating());
	Timer total_timer{"Total"};

	tracking.reset({74.1952, 8.29092, 1.40752});

	setFlywheelVel(barrier_rpm - 50);
	shoot(9, true);	
	shooter.waitToReachState(ShooterIdleParams{});

	moveToTargetSync({tracking.getPos().x + 10, tracking.getPos().y + 3});
	turnToAngleSync(90);
	moveToTargetSync({32, 15});

	turnToAngleSync(0);


// SKILLS START ---------


	// WAIT_UNTIL(!gyro.isCalibrating());
	setFlywheelVel(2115);

	flattenAgainstWallSync();
	tracking.reset(distanceReset(resetPosition::leftHome));
	spinRoller(600);
	intake.waitToReachState(IntakeOffParams{});
	turnToTargetSync({35.0, 205.0});

	// moveToTargetSync({tracking.getPos().x, 25.0}, E_Brake_Modes::coast);	// HITS STACK

	intakeOn();
	moveToTargetSync({35.0, 45.0}, E_Brake_Modes::brake, 70);
	aimAtBlue(4);
	driveBrake();
	shoot(3);
	shooter.waitToReachState(ShooterIdleParams{});

	setFlywheelVel(1820);
	moveToTargetSync({68.0, 90.0}, E_Brake_Modes::brake, 100);	// Go to barrier
	aimAtBlue(3);
	shoot(3);
	shooter.waitToReachState(ShooterIdleParams{});


	turnToTargetSync({114, 125});	// Go to corner
	intakeOn();
	moveToTargetSync({114, 125}, E_Brake_Modes::brake, 90);
	turnToAngleSync(180);


// SECTION 2 -------------------------------------------


	setFlywheelVel(2300);
	flattenAgainstWallSync();
	tracking.reset(distanceReset(resetPosition::leftHome));
	spinRoller(600);
	intake.waitToReachState(IntakeOffParams{});
	turnToTargetSync({35.0, 23.0});
	moveToTargetSync({35.0, 23.0}, E_Brake_Modes::coast);
	aimAtBlue(2);
	delay(500);	// WAITS FOR LAST DISC TO INTAKE
	shoot(3);
	shooter.waitToReachState(ShooterIdleParams{});
	
	// Goes to stack
	setFlywheelVel(2045);
	turnToTargetSync({35.0, 49.0});
	moveToTargetSync({35.0, 49.0});
	aimAtBlue(2);
	driveBrake();
	shoot(3);
	shooter.waitToReachState(ShooterIdleParams{});

	setFlywheelVel(1850);
	// turnToTargetSync({64.0, 84.0});
	moveToTargetSync({60.0, 82.0}, E_Brake_Modes::brake, 100);	// Go to barrier
	aimAtBlue(2);
	shoot(3);
	shooter.waitToReachState(ShooterIdleParams{});

	moveToTargetSync({tracking.getPos().x + 10.0, tracking.getPos().y - 10.0});
	turnToTargetSync({125.0, 125.0});
	intakeOn();
	moveToTargetSync({120.0, 120.0}, E_Brake_Modes::brake, 100);
	moveToTargetSync({112.0, 112.0});

	turnToAngleSync(-90);
	flattenAgainstWallSync();


// SECTION 3 -------------------------------------------

	flattenAgainstWallSync();
	setFlywheelVel(2075);
	tracking.reset(distanceReset(resetPosition::rightHome));
	spinRoller(600);
	intake.waitToReachState(IntakeOffParams{});

	// Goes to stack
	moveToTargetSync({47.0, tracking.getPos().y});

	aimAtRed(3);
	shoot(3);
	shooter.waitToReachState(ShooterIdleParams{});

	// Goes to barrier corner
	setFlywheelVel(1750);
	intakeOn();

	turnToTargetSync({72.0, 40.0});
	moveInches(30.0);
	aimAtRed(3);
	shoot(3);
	shooter.waitToReachState(ShooterIdleParams{});

	// Goes field corner

	turnToTargetSync({24.0, 24.0}, 0.0, true);
	moveToTargetSync({24.0, 24.0}, E_Brake_Modes::brake, 127, 2.0);
	turnToAngleSync(45.0);

	master.print(2, 0, "total: %d", total_timer.getTime());
}

void provSkillsLow(){
	WAIT_UNTIL(!gyro.isCalibrating());
	Timer total_timer{"Total"};
// /*
	tracking.reset({74.1952, 8.29092, 1.40752});

	setFlywheelVel(barrier_rpm - 25);
  delay(1000);
	shoot(9, true);	
	shooter.waitToReachState(ShooterIdleParams{});
  shoot(3);	
	shooter.waitToReachState(ShooterIdleParams{});



	moveToTargetSync({tracking.getPos().x + 10, tracking.getPos().y + 3});
	turnToTargetSync({33, 17}, 0.0, true);
	moveToTargetSync({33, 17});

	turnToAngleSync(0);


// SKILLS START (from roller) ---------


	setFlywheelVel(1825);

	flattenAgainstWallSync();
	tracking.reset(distanceReset(resetPosition::leftHome));
	spinRoller(600);
	intake.waitToReachState(IntakeOffParams{});
	turnToTargetSync({35.0, 46.0});

	intakeOn();
	moveToTargetSync({35.0, 46.0}, E_Brake_Modes::brake, 55);
  delay(500); // Intakes last disc
	intakeOff();

	// aimAtBlue(4);
	// driveBrake();
	// shoot(1);
	// shooter.waitToReachState(ShooterIdleParams{});
  // delay(100000);

  // moveToTargetSync({35.0, 72.0}); // pushes disks
  moveInches(25.0);
  intakeRev();
  moveInches(-1);
	aimAtBlue(2);
	driveBrake();
  shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});

  // intakeOn();
  // moveInches(4.0); // intakes disks
	// aimAtBlue(2);
  // shoot(1);
  // shooter.waitToReachState(ShooterIdleParams{});
  // delay(100000);



  // Goes to Roller

  turnToTargetSync({15.0, 30.0}, 0.0, true);
  moveToTargetSync({15.0, 30.0});
  turnToAngleSync(90);

	flattenAgainstWallSync();
  tracking.reset(distanceReset(resetPosition::rightHome));
	spinRoller(600);
	intake.waitToReachState(IntakeOffParams{});

  turnToTargetSync({22.0, 22.0});
  moveToTargetSync({22.0, 22.0});
  turnToAngleSync(45.0);

  endgame_s_p.setState(HIGH);
  endgame_d_p.setState(HIGH);
}
