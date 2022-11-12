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


// ------------------------------------------------ AUTONS ------------------------------------------------
void auton1(){
	xDriveMTT(0+gX, 30+gY, -45+gA, E_Brake_Modes::brake, 1);
	delay(1000);
	xDriveMTT(-20+gX, 35+gY, -45, E_Brake_Modes::brake, 0.5);
	delay(1000);
	xDriveTTA(-45, E_Brake_Modes::brake, 1);
}
void auton2(){
	xDriveMTT(0+gX, 16+gY, 45+gA, E_Brake_Modes::brake, 1);
	screen::print(pros::E_TEXT_LARGE, 1, "Finishde First movment");
	delay(500);
	xDriveMTT(44+gX, 44+gY, 45+gA, E_Brake_Modes::brake, 1);
	xDriveTTT(0, 96, E_Brake_Modes::brake, 1);
}
void auton3(){
	xDriveMTT(-20+gX, 20+gY, 90+gA, E_Brake_Modes::brake, 1);
}
void autonM(){
	int temp = millis();
	delay(1000);
	xDriveMTT(24, 24, -30, E_Brake_Modes::brake, 0.5, 127);
	delay(500);
	xDriveTTT(24, 120, E_Brake_Modes::brake, 0.5);
	delay(1000);
	xDriveMTT(40, gY, 25, E_Brake_Modes::brake, 0.5, 127);
	xDriveMTT(72, 48, 45, E_Brake_Modes::brake, 0.5, 50);
	delay(1000);
	xDriveTTT(24, 120, E_Brake_Modes::brake, 0.5);
	delay(1500);
	screen::print(pros::E_TEXT_MEDIUM, 1, "%d", millis()-temp);
}
void autonM2(){
	int temp = millis();
	delay(1000);
	xDriveMTT(120, 120, -45, E_Brake_Modes::brake, 0.5, 127);
	delay(500);
	xDriveTTT(24, 120,E_Brake_Modes::brake, 0.5);
	delay(1500);
	xDriveMTT(120, 96, -135, E_Brake_Modes::brake, 0.5, 80);
	xDriveMTT(76, 45, -135, E_Brake_Modes::brake, 0.5, 127);
	delay(500);
	xDriveTTT(24, 120,E_Brake_Modes::brake, 0.5);
	delay(1500);
	screen::print(pros::E_TEXT_MEDIUM, 1, "%d", millis()-temp);
}