#include "auton.hpp"
#include "tracking.hpp"
#include "Subsystems/intake.hpp"
#include "Subsystems/flywheel.hpp"
#include "Subsystems/shooter.hpp"

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
  Timer total{"total_timer"};
  setFlywheelVel(2300);
  spinRoller();;
  intake.waitToReachState(IntakeOffParams{});
  // tracking.reset({30.75, 7.375, degToRad(0.0)});
  master.rumble("-");
  intakeOn();
  moveToTargetSync({35.25, 43.0}); // picks up stack
  aimAtBlue();
    // shoot(3);
  delay(1500);  // shoots stack
  moveToTargetSync({62.0, 85.0}); // picks up line of discs
  aimAtBlue(0);
    // shoot(3);
  delay(1500);  // shoots line of discs
  
  // turnToAngleSync(180);
  // spinRoller();
  // intake.waitToReachState(IntakeOffParams{});
	log("total: %d", total.getTime());
	lcd::print(7, "total: %d", total.getTime());
}

// tracking.g_pos = {68.0, 129.25, M_PI};
void skills2(){
  Timer total{"total_timer"};
	log("total: %d", total.getTime());
	lcd::print(7, "total: %d", total.getTime());
  // shoot match loads here
}
// tracking.g_pos = {72.0, 11.25, 0.0};
void skills3(){
  Timer total{"total_timer"};
 
  log("total: %d", total.getTime());
	lcd::print(7, "total: %d", total.getTime());
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
  tracking.reset({128.5, 83.5, degToRad(0.0)});
  
  Timer timer1{"timer"};
  setFlywheelVel(2440);
  moveToTargetSync({127, 110});
  turnToAngleSync(-90.0);
  moveInches(-2.0); // Backup in front of roller
  spinRoller();
  intake.waitToReachState(IntakeOffParams{});
  tracking.reset({133.5, tracking.g_pos.y, -M_PI_2});
  moveToTargetSync({128.0, 111.0});
  aimAtBlue(10.5);
  shoot(2);
  shooter.waitToReachState(ShooterIdleParams{});

  setFlywheelVel(2270);
  turnToTargetSync({83.0, 59.0});
  intakeOn();
  moveToTargetSync({83.0, 59.0});
  aimAtBlue(10.5);
  shoot(3);
  shooter.waitToReachState(ShooterIdleParams{});

  master.print(2,0, "total:%ld", timer1.getTime());
	lcd::print(6, "total:%ld", timer1.getTime());
}