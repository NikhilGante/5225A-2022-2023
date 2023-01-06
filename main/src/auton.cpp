#include "auton.hpp"
#include "tracking.hpp"
#include "Subsystems/intake.hpp"
#include "Subsystems/flywheel.hpp"
#include "Subsystems/shooter.hpp"

// start coord 
// tracking.g_pos = {30.75, 7.375, degToRad(0.0)};	// ACTUAL SKILLS
void skills1(){
  Timer total{"total_timer"};
  setFlywheelVel(2300);
  spinRoller();
  printf("whatup\n");
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

