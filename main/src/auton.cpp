#include "auton.hpp"

// start coord 
// tracking.g_pos = {35.25, 11.25, 0.0};
void skills1(){
  Timer total{"total_timer"};
	moveToTarget({35.25, 40.0});	// pickup stack of discs
	turnToAngle(90.0);
	moveToTarget({13.0, 36.0}, E_Brake_Modes::brake, 127, 2.0); // go to roller
	delay(1000); // reset

	moveToTarget({18.0, 37.0});	// move forward from wall
	aimAtRed();
	delay(1500);
	turnToTarget({60.0, 84.0});
	moveToTarget({60.0, 84.0}); // pickup line of discs
	aimAtRed();
	delay(1500);
	turnToTarget({90.0, 114.0});
	moveToTarget({90.0, 114.0}, E_Brake_Modes::none, 80);	// picks up stack
	// aimAtRed();
	// delay(1500);
	turnToTarget({71.0, 126.5}, true);
	moveToTarget({71.0, 126.5}); // goes to match loader
	turnToAngle(180);
	delay(1000); // reset
	log("total: %d", total.get_time());
	lcd::print(7, "total: %d", total.get_time());
}

// tracking.g_pos = {68.0, 129.25, M_PI};
void skills2(){
  Timer total{"total_timer"};
	moveToTarget({68.00, 125.5}); // moves away from wall
	aimAtRed();
  delay(6000);
  turnToTarget({118.0, 102.0});
  moveToTarget({118.0, 102.0}, E_Brake_Modes::none, 80.0); // intake stack
	aimAtBlue();
  delay(1500);
  
  turnToTarget({78.0, 54.0});
  moveToTarget({78.0, 54.0}); // go to blue low goal
	aimAtBlue();
  delay(1500);
  turnToTarget({54.0, 30.0});
  moveToTarget({54.0, 30.0}, E_Brake_Modes::none);
  turnToTarget({67.0, 13.0}, true);
  moveToTarget({67.0, 13.0}, E_Brake_Modes::none);
  turnToAngle(0.0);
  delay(1000);  // reset

	log("total: %d", total.get_time());
	lcd::print(7, "total: %d", total.get_time());
  // shoot match loads here
}
// tracking.g_pos = {72.0, 11.25, 0.0};
void skills3(){
  Timer total{"total_timer"};
  moveToTarget({72.0, 15.0});
  aimAtBlue();
  delay(5000);
  moveToTarget({24.0, 24.0});
  turnToAngle(45.0);
  log("total: %d", total.get_time());
	lcd::print(7, "total: %d", total.get_time());
  // expand
}