#pragma once
#include "main.h"
#include "../config.hpp"
#include "controller.hpp"
#include "task.hpp"


using namespace std;
using namespace pros;






class ControllerButton{
private:
	Controller& controller;
	controller_digital_e_t button;
	int last_press = 0; int L_last_press;
	int last_release = 0; int L_last_release;
	static _Task_ task;
	bool prev = false;
	bool cur;
	static vector<ControllerButton*> ButtonsArray;
public:
	ControllerButton (Controller& controller, controller_digital_e_t button);
	static void updateLoop();
	void update();
	static void init();


	// ------------------------------------------ Press Functions ------------------------------------------
	bool holdClick() const;
	bool doubleClick() const;
	bool tripleClick() const;

	bool isPressed() const;
	

};


// Buttons
extern ControllerButton 
ButtonA,
ButtonX, 
ButtonY, 
ButtonB, 
ButtonL1, 
ButtonL2, 
ButtonR1, 
ButtonR2, 
ButtonUp, 
ButtonDown, 
ButtonLeft, 
ButtonRight;
