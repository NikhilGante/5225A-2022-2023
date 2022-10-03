#include "ControllerButtons.hpp"


// Buttons Array
vector<ControllerButton*> ControllerButton::ButtonsArray;
_Task_ ControllerButton::task{"ControllerButton"};


// Buttons Objects
ControllerButton ButtonA(master, DIGITAL_A);
ControllerButton ButtonX(master, DIGITAL_X);
ControllerButton ButtonY(master, DIGITAL_Y);
ControllerButton ButtonB(master, DIGITAL_B);
ControllerButton ButtonL1(master, DIGITAL_L1);
ControllerButton ButtonL2(master, DIGITAL_L2);
ControllerButton ButtonR1(master, DIGITAL_R1);
ControllerButton ButtonR2(master, DIGITAL_R2);
ControllerButton ButtonUp(master, DIGITAL_UP);
ControllerButton ButtonDown(master, DIGITAL_DOWN);
ControllerButton ButtonLeft(master, DIGITAL_LEFT);
ControllerButton ButtonRight(master, DIGITAL_RIGHT);


ControllerButton::ControllerButton(Controller& controller, controller_digital_e_t button):
controller(controller), button(button)
{
  //if (ButtonsArray[button-controller_digital_e_t::E_CONTROLLER_DIGITAL_L1]) printf("You are declaring another button, Overwriting Button!");
  ButtonsArray.push_back(this);
}

void ControllerButton::init(){
  pros::delay(500);
  task.start(updateLoop);
}


void ControllerButton::updateLoop(){
  while (true){
    for(ControllerButton* button: ButtonsArray) button->update();
    _Task_::delay(25);
  }
}

void ControllerButton::update(){
  
  master.get_digital(button); 
  cur = controller.get_digital(button); 
  if (cur && ! prev){
    L_last_press = last_press;
    last_press = pros::c::millis();
    // --------------------- Log Function ---------------------
  }
  
  else if (!cur && prev) {
    L_last_release = last_release;
    last_release = pros::c::millis();
    // ------------------------------------------ Log Function ---------------------
  }
  prev = cur;
}




// ------------------------------------------ Press Functions ------------------------------------------
bool ControllerButton::holdClick(){
  if (last_press>last_release && pros::c::millis()>last_press+250 && pros::c::millis()<last_press+400) return true;
  else return false;
}

bool ControllerButton::doubleClick(){
  if (last_press>last_release && abs(last_press-last_release)<200) return true;
  else return false;
}

bool ControllerButton::tripleClick(){
  if (last_press>last_release && abs(last_press-last_release)<200 && abs(last_release-L_last_press)<200 && abs(L_last_press-L_last_release)<200) return true;
  else return false;
}

/*
L_last_release = 50
L_last_press = 100
last_release = 150
last_press = 200

check code here

release is here....
*/