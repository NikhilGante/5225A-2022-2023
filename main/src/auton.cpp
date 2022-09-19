#include "auton.hpp"

// start coord 
// tracking.g_pos = {35.25, 11.25, 0.0};
void skills1(){
  Timer total{"total_timer"};
	moveToTargetSync({35.25, 40.0});	// pickup stack of discs
	turnToAngleSync(90.0);
	moveToTargetSync({13.0, 36.0}, E_Brake_Modes::brake, 127, 2.0); // go to roller
	delay(1000); // reset

	moveToTargetSync({18.0, 37.0});	// move forward from wall
	aimAtRed();
	delay(1500);
	turnToTargetSync({60.0, 84.0});
	moveToTargetSync({60.0, 84.0}); // pickup line of discs
	aimAtRed();
	delay(1500);
	turnToTargetSync({90.0, 114.0});
	moveToTargetSync({90.0, 114.0}, E_Brake_Modes::none, 80);	// picks up stack
	// aimAtRed();
	// delay(1500);
	turnToTargetSync({71.0, 126.5}, true);
	moveToTargetSync({71.0, 126.5}); // goes to match loader
	turnToAngleSync(180);
	delay(1000); // reset
	log("total: %d", total.get_time());
	lcd::print(7, "total: %d", total.get_time());
}

// tracking.g_pos = {68.0, 129.25, M_PI};
void skills2(){
  Timer total{"total_timer"};
	moveToTargetSync({68.00, 125.5}); // moves away from wall
	aimAtRed();
  delay(6000);
  turnToTargetSync({118.0, 102.0});
  moveToTargetSync({118.0, 102.0}, E_Brake_Modes::none, 80.0); // intake stack
	aimAtBlue();
  delay(1500);
  
  turnToTargetSync({78.0, 54.0});
  moveToTargetSync({78.0, 54.0}); // go to blue low goal
	aimAtBlue();
  delay(1500);
  turnToTargetSync({54.0, 30.0});
  moveToTargetSync({54.0, 30.0}, E_Brake_Modes::none);
  turnToTargetSync({67.0, 13.0}, true);
  moveToTargetSync({67.0, 13.0}, E_Brake_Modes::none);
  turnToAngleSync(0.0);
  delay(1000);  // reset

	log("total: %d", total.get_time());
	lcd::print(7, "total: %d", total.get_time());
  // shoot match loads here
}
// tracking.g_pos = {72.0, 11.25, 0.0};
void skills3(){
  Timer total{"total_timer"};
  moveToTargetSync({72.0, 15.0});
  aimAtBlue();
  delay(5000);
  moveToTargetSync({24.0, 24.0});
  turnToAngleSync(45.0);
  log("total: %d", total.get_time());
	lcd::print(7, "total: %d", total.get_time());
  // expand
}

// tracking.g_pos = {35.25, 11.25, 0.0};
void auton50pts(){
  Timer total{"total_timer"};
  moveToTargetSync({35.25, 15.0});  // move away from wall
  turnToTargetSync({72.0, 48.0});
  moveToTargetSync({72.0, 48.0}, E_Brake_Modes::brake, 80.0, 1.5); // pickup stack
  aimAtRed();
  delay(1500);
  WAIT_UNTIL(master.get_digital_new_press(DIGITAL_A));
  turnToTargetSync({132.0, 112.0});
  moveToTargetSync({132.0, 112.0}); // pickup line of 3 discs
  turnToAngleSync(-90.0); // face wall
  delay(1000);  // reset
  delay(2500); // move and face red goal
  delay(1500);// shoot 3 discs
  log("total: %d", total.get_time());
	lcd::print(7, "total: %d", total.get_time());
}