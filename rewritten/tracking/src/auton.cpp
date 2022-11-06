#include "auton.hpp"


// Auton Class stuff
void auton::program1(){
	auton::selectAuton();
	auton::setAutonValues();
	auton::adjustRobot();
	auton::save();
}
void auton::program2(){
	master.clear();
	auton::readValues();
	auton::confirmValues();
	auton::runAuton();
}
vector<auton*> auton::array{};
int auton::curAuton = 0;


