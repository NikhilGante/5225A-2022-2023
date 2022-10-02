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
	log("total: %d", total.getTime());
	lcd::print(7, "total: %d", total.getTime());
}

// tracking.g_pos = {68.0, 129.25, M_PI};
void skills2(){
  Timer total{"total_timer"};
	moveToTargetSync({68.00, 125.5}); // moves away from wall
	aimAtBlue();
  WAIT_UNTIL(master.get_digital_new_press(DIGITAL_A));
  // delay(6000);
  turnToTargetSync({123.0, 98.0});
  moveToTargetSync({123.0, 98.0}, E_Brake_Modes::none, 80.0); // intake stack
	aimAtRed();
  delay(1500);
  
  turnToTargetSync({76.0, 53.0});
  WAIT_UNTIL(master.get_digital_new_press(DIGITAL_A));
  moveToTargetSync({76.0, 53.0}); // go to blue low goal
	aimAtRed();
  // delay(1500);
  WAIT_UNTIL(master.get_digital_new_press(DIGITAL_A));
  turnToTargetSync({52.0, 28.0});
  moveToTargetSync({52.0, 28.0}, E_Brake_Modes::none);
  turnToTargetSync({67.0, 13.0}, true);
  moveToTargetSync({67.0, 13.0}, E_Brake_Modes::none);
  turnToAngleSync(0.0);
  delay(1000);  // reset

	log("total: %d", total.getTime());
	lcd::print(7, "total: %d", total.getTime());
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
  log("total: %d", total.getTime());
	lcd::print(7, "total: %d", total.getTime());
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
  log("total: %d", total.getTime());
	lcd::print(7, "total: %d", total.getTime());
}

// NEW SKILLS

// tracking.g_pos = {34.75, 11.25, degToRad(0.0)};
void new_skills1(){ // 12 seconds
  Timer total{"total_timer"};
  moveToTargetSync({34.75, 46.00}, E_Brake_Modes::brake, 65);
  aimAtBlue();
  delay(1500);// shoot 3 discs
  WAIT_UNTIL(master.get_digital_new_press(DIGITAL_A));
  moveToTargetSync({61.00, 88.00}, E_Brake_Modes::brake, 127, 2.5); // intake line of 3 discs
  aimAtBlue();
  delay(1500);// shoot 3 discs
  WAIT_UNTIL(master.get_digital_new_press(DIGITAL_A));
  turnToTargetSync({109.0, 129.0}); 
  moveToTargetSync({109.0, 129.0}, E_Brake_Modes::brake, 80); // intake stack of discs
  turnToAngleSync(180);
  delay(1000);  // "reset"

  log("total: %d", total.getTime());
	lcd::print(7, "total: %d", total.getTime());
}


// tracking.g_pos = {108.0, 129.75, degToRad(180.0)};
void new_skills2(){ // 17 seconds
  Timer total{"total_timer"};
  moveToTargetSync({104.0, 117.0}); // move away from wall
  aimAtRed();
  delay(1500);  // shoot 3 discs
  moveToTargetSync({112.0, 99.0});  // intake stack
  aimAtRed();
  delay(1500);  // shoot 3 discs
  moveToTargetSync({83.0, 57.0});   // intake line of 3 discs
  aimAtRed();
  delay(1500);  // shoot 3 discs

  moveToTargetSync({75.0, 65.0});   // backup on to centreline
  turnToTargetSync({20.0, 20.0});   // turn to face line
  moveToTargetSync({20.0, 20.0}, E_Brake_Modes::brake, 80, 2.0);   // go to corner

  turnToAngleSync(90.0);  // face wall
  delay(2000); // reset

  log("total: %d", total.getTime());
	lcd::print(7, "total: %d", total.getTime());
}

// tracking.g_pos = {11.25, 23.5, degToRad(90.0)};
void new_skills3(){ // 14 seconds
  Timer total{"total_timer"};
  moveToTargetSync({48.0, 35.0});   // go to stack
  aimAtRed();
  delay(1500);  // shoot 3 discs
  moveToTargetSync({80.0, 35.0});   // intake stack
  aimAtRed();
  delay(1500);  // shoot 3 discs

  turnToTargetSync({73.0, 63.0});   // face centreline
  moveToTargetSync({73.0, 63.0}, E_Brake_Modes::coast);   // go to centreline

  moveToTargetSync({118.0, 118.0}, E_Brake_Modes::brake, 80);   // go to corner
  turnToAngleSync(-90); // face wall
  moveToTargetSync({127.0, 118.0}); // move to wall

  delay(1000); // reset

  log("total: %d", total.getTime());
	lcd::print(7, "total: %d", total.getTime());
}

// tracking.g_pos = {116.5, 116.5, degToRad(-90.0)};
void new_skills4(){ // takes 5 seconds
  Timer total{"total_timer"};
  moveToTargetSync({127.0, 118.0}); // move away from wall
  aimAtRed();
  delay(1500);  // shoot 3 discs
  turnToTargetSync({118.0, 118.0}, false);
  moveToTargetSync({118.0, 118.0}, E_Brake_Modes::brake, 80.0); // goes to corner tile
  turnToAngleSync(-135.0); // face centreline
  log("total: %d", total.getTime());
	lcd::print(7, "total: %d", total.getTime());
  // expand
}
