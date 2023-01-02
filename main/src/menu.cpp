#include "menu.hpp"

int auton_count = 0;

// Auton Class stuff
void auton::program1(){
	auton::selectAuton();
	// auton::setAutonValues();
	// auton::adjustRobot();
	// auton::save();
}
void auton::program2(){
	master.clear();
	auton::readValues();
	auton::confirmValues();
	auton::runAuton();
}
array<auton*, AUTON_ARR_SIZE> auton::arr;
int auton::curAuton = 0;


// ------------------------------------------------ AUTONS ------------------------------------------------

