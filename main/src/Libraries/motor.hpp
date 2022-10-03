#pragma once
#include "main.h"
#include "../config.hpp"

using namespace std;
using namespace pros;




class MOTOR: public pros::Motor{
private:
	std::string name;
	int temp_max;
	int jammed;
	int jamFunction;
public:
	int prev_voltage = 0;
	bool jam_check_task;
	static vector<MOTOR*> array;
	
	enum jam {
		stop = 0, 
		reverse = 1,
		resume = 2,
	};
	

public:
	MOTOR (std::string name, int port, bool Reverse, pros::motor_gearset_e gearset, MOTOR::jam jamF, int temp_max = 45, int jammed = 25);
    //pros::Motor(port, gearset, Reverse, pros::E_MOTOR_ENCODER_DEGREES);


	void TempatureCheck();
	bool IsJammed();
	void move(std::int32_t voltage);
	void changeJamFunction(MOTOR::jam jamF);

	void JamFunction();

	// This function is not part of every object created. When called, it will JamCheck every MOTOR ob
	static void JamCheck(); 
	
};