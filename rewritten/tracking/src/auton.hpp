#pragma once
#include "tracking.hpp"
#include <fstream>
#include "config.hpp"
using namespace pros;
using namespace std;

// --------------------------------------------- Auton run ---------------------------------------------


void auton1();
void auton2();
void auton3();
void autonM();
void autonM2();


#define FIELD_MAX_MM 3581.4
#define DISTANCE_OFF_MIDDLE 220.218
#define DISTANCE_SENSOR_OFFSET 0
#define USE_DISTANCE_SENSOR -1

class auton{
public:
	std::string name;
	double x; // Each auton has a starting x(If negative, it will be calculated by distance sensor)
	double y; // Each auton has a starting y(If negative, it will be calculated by distance sensor)
	double a; // Each auton has a starting a
	bool distanceX = false; // When false, it will not use a distance sensor to calcualte the y(It will turn true if a negative number is passed as a parameter in the constructur)
	bool distanceY = false;
	bool zeroOriented; // This says if the distance sensor being used to calculate the position is point at 0 or not
	std::function<void(void)> autonProgram; // Function the auton will run
	pros::Distance& distanceSensor; // Distance sensor used for that auton
	

	static int curAuton; // Current auton Selected
	static vector<auton*> array; // Array of different autons


	auton(std::string name, double autonX, double autonY, double autonA, std::function<void(void)> autonProgram, pros::Distance& ds, bool zeroOriented = true): 
	name(name), 
	x(autonX), 
	y(autonY), 
	a(autonA), 
	autonProgram(autonProgram),
	distanceSensor(ds),
	zeroOriented(zeroOriented)
	{
		array.push_back(this); // Adding this auton to array
		if (x<0) distanceX = true; // Checking if you should use the distance sensor for x
		if (y<0) distanceY = true; // Checking if you should use the distance sensor for y 
	}
	static void program1();
	static void program2();
public:
	// Methods --------------------------------------------------------------------------------------------------
	static auton* GetCurAuton(){return array[curAuton];} // Returns a pointer to the current Auton Running
	static void increase(){ // This is for auton selection, Scrolls through auton Array 
		if (curAuton == array.size() - 1) curAuton = 0;
		else curAuton++;
	}
	static void decrease() { // This is for auton selection, Scrolls through auton Array
		if (curAuton == 0) curAuton = array.size() - 1;
		else curAuton--;
	}
	static double distance(){ // Calculates the distance from wall using distance sensor, and if its zero oriented
		if (GetCurAuton()->zeroOriented) return (GetCurAuton()->distanceSensor.get()+DISTANCE_SENSOR_OFFSET+DISTANCE_OFF_MIDDLE)/25.4;
		else return (FIELD_MAX_MM-((GetCurAuton()->distanceSensor.get()+DISTANCE_SENSOR_OFFSET+DISTANCE_OFF_MIDDLE)))/25.4;
	}
	static void calculateAutonValues(){ // Recalculates auton values for distance sensors
		if (GetCurAuton()->distanceX) GetCurAuton()->x = distance();
		if (GetCurAuton()->distanceY) GetCurAuton()->y = distance(); 
	}
	static void selectAuton(){ // Loops through autons on controller and lets you select one
		master.clear();
		delay(100);
		while (!master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
			if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_LEFT)) decrease();
			else if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_RIGHT)) increase();
			//cout << GetCurAuton()->name << endl;
			master.print(0, 0, "%s           ", GetCurAuton()->name.c_str());

			pros::delay(50);
		}
	}
	static void setAutonValues(){ // Click x to set auton values, Allows you to redo incase distance sensors give invalid readings
		master.clear();
		delay(100);
		while (!master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
			master.print(0, 5, "%s", "ResetValues");
			delay(50);
			master.print(1, 0, "%.2f", float(GetCurAuton()->x));
			delay(50);
			master.print(1, 7, "%.2f", GetCurAuton()->y);
			delay(50);
			master.print(1, 15, "%.2f", GetCurAuton()->a);
			delay(50);
			if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_X)) calculateAutonValues();
		}
		resetTracking(GetCurAuton()->x, GetCurAuton()->y, GetCurAuton()->a);
	}
	static void resetTracking(double x, double y, double a){ // Resets tracking with given values
		gX = x;
        gY = y;
        gA = a*M_PI/180;
	}
	static void adjustRobot(){ // Allows you to move the robot to your likings with tracking enabled. Then saves the tracking values to auton members
		Task my_task(tracking);
		master.clear();
		delay(100);
		while (!master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
			master.print(0, 0, "%lf", gX); 
			delay(50);
			master.print(1, 0, "%lf", gY);
			delay(50);
			master.print(2, 0, "%lf", (gA*180/M_PI));
			delay(50);
		}
		GetCurAuton()->x = gX;
		GetCurAuton()->y = gY;
		GetCurAuton()->a = gA*180/M_PI;
	}
	static void save(){ // Saves auton values to the sd Card
		if (!usd::is_installed()) cout << "No SD Card" << endl;
		else{
			std::ofstream auton_file ("/usd/auton.txt");
			auton_file << curAuton << endl;
			auton_file << GetCurAuton()->x << endl;
			auton_file << GetCurAuton()->y << endl;
			auton_file << GetCurAuton()->a << endl;
			auton_file << GetCurAuton()->name << endl;
			auton_file.close();
			cout << "Auton Saved To SD Card" << endl;
		}
	}
	static void readValues(){ // Reads valuss from sd card and prints them onto controller. Allows user to download values
		delay(50);
		master.print(0, 0, "Download Valules?");
		while (!master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_A)) {
			delay(50);
		}
		if (!usd::is_installed()) cout << "No sd Card" << endl;
		else{
			std::ifstream auton_file ("/usd/auton.txt");
			auton_file >> curAuton;
			auton_file >> GetCurAuton()->x;
			auton_file >> GetCurAuton()->y;
			auton_file >> GetCurAuton()->a;
			auton_file.close();
			cout << "Values Downloaded" << endl;
		}
	}
	static void confirmValues(){ // User confirms values
		master.clear();
		while (!master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
			//cout << "Confirmig values" << ' ' << GetCurAuton()->x << ' ' << GetCurAuton()->y << ' ' << GetCurAuton()->a << endl;
			master.print(0, 0, "%lf            ", GetCurAuton()->x);
			delay(50);
			master.print(1, 0, "%lf            ", GetCurAuton()->y);
			delay(50);
			master.print(2, 0, "%lf            ", GetCurAuton()->a);
			delay(50);
		}
	}
	static void runAuton(){ // User runs auton
		printf("Auton Running");
		GetCurAuton()->autonProgram();
	}


};




