#include "controller.hpp"
#include "task.hpp"
#include "timer.hpp"
#include "../util.hpp"

// Buttons

  // ------------------------------ Direction Buttons ------------------------------
  controller_digital_e_t goalDisturbBtn = DIGITAL_RIGHT;
  // Up and Down Taken by Adjusting Disc count

  // ------------------------------ Letter Buttons ------------------------------
  controller_digital_e_t intakeToggleBtn = DIGITAL_A;
  controller_digital_e_t intakeRevBtn = DIGITAL_X;
  controller_digital_e_t angleOverrideBtn = DIGITAL_B;
  controller_digital_e_t endgameBtn = DIGITAL_Y;

  // ------------------------------ Front Buttons ------------------------------
  controller_digital_e_t anglerToggleBtn = DIGITAL_L1;
  controller_digital_e_t transToggleBtn = DIGITAL_L2;
  controller_digital_e_t singleShotBtn = DIGITAL_R1;
  controller_digital_e_t tripleShotBtn = DIGITAL_R2;

//Static Member Initializations
_Task _Controller::controller_task{"Controller"};
_Controller* _Controller::master_ptr{nullptr};
_Controller* _Controller::partner_ptr{nullptr};

_Controller::_Controller(pros::controller_id_e_t id): pros::Controller{id}{
  switch(id){
    case CONTROLLER_MASTER:
      master_ptr = this;
      name = "Master";
    break;
    case CONTROLLER_PARTNER:
      partner_ptr = this;
      name = "Partner";
    break;
  }
  queue.changeName("Controller" + name);
}

void _Controller::handle(){
  if(!queue.empty()){
    controller_log("Running function on %s controller\n", name);
    queue.front()();
    queue.pop();
    _Task::delay(50);
  }
}

void _Controller::init(){
  controller_task.start([](){
    while(true){
      if(master_ptr) master_ptr->handle();
      if(partner_ptr) partner_ptr->handle();
      _Task::delay();
    }
  });
}

void _Controller::clearLine (std::uint8_t line){
  queue.push([=, this](){
    pros::Controller::clear_line(line);
    controller_log("clearing line %d on %s controller", line, name);
  });
  controller_log("Adding clearLine for %s controller", name);
}

void _Controller::clear(){
  queue.push([=, this](){
    pros::Controller::clear();
    controller_log("Clearing screen on %s controller", name);
  });
  controller_log("Adding clear to %s controller queue", name);
}

void _Controller::rumble(std::string rumble_pattern){
  queue.push([=, this](){
    pros::Controller::rumble(rumble_pattern.c_str());
    controller_log("Rumbling %s controller", name);
  });
  controller_log("Adding rumble to %s controller queue", name);
}

void _Controller::wait_for_press(controller_digital_e_t button, int timeout) {wait_for_press(std::vector{button}, timeout);}

controller_digital_e_t _Controller::wait_for_press(std::vector<controller_digital_e_t> buttons, int timeout){
  Timer timer{"Controller Button Timeout"};
  controller_log("Waiting for button press on %s controller with a timeout of %d", name, timeout);
  
  WAIT_UNTIL(timer.getTime() > timeout){
    for(controller_digital_e_t btn: buttons){
      if(get_digital_new_press(btn)){
        controller_log("Button %d pressed on %s controller", btn, name);
        return btn;
      }
    }
  }
  controller_log("Timed out on waiting for button press from controller %s", name);
  return static_cast<controller_digital_e_t>(0);
}

int _Controller::getAnalog(controller_analog_e_t joystick){
  return getAnalog(joystick, deadzone);
}

int _Controller::getAnalog(controller_analog_e_t joystick, int deadzone){
  int value = get_analog(joystick);
  return std::abs(value) > deadzone ? value : 0;
}

std::string _Controller::getText(int line) const{
  switch(line){
    case 0: return line_0; break;
    case 1: return line_1; break;
    case 2: return line_2; break;
    default: return "Invalid line"; break;
  }
}

std::string& _Controller::getText(int line){
  static std::string error = "Invalid line";
  switch(line){
    case 0: return line_0; break;
    case 1: return line_1; break;
    case 2: return line_2; break;
    default: return error; break;
  }
}


bool _Controller::interrupt(bool analog, bool digital, bool OK_except){
  if (analog){
    if (get_analog(ANALOG_LEFT_X ), 20) return true;
    if (get_analog(ANALOG_LEFT_Y ), 20) return true;
    if (get_analog(ANALOG_RIGHT_X), 20) return true;
    if (get_analog(ANALOG_RIGHT_Y), 20) return true;
  }
  if(digital){
    if (get_digital(okBtn)) return !OK_except;
    if (get_digital(DIGITAL_A)) return true;
    if (get_digital(DIGITAL_B)) return true;
    if (get_digital(DIGITAL_Y)) return true;
    if (get_digital(DIGITAL_X)) return true;
    if (get_digital(DIGITAL_R1)) return true;
    if (get_digital(DIGITAL_R2)) return true;
    if (get_digital(DIGITAL_L1)) return true;
    if (get_digital(DIGITAL_L2)) return true;
    if (get_digital(DIGITAL_RIGHT)) return true;
    if (get_digital(DIGITAL_DOWN)) return true;
    if (get_digital(DIGITAL_LEFT)) return true;
    if (get_digital(DIGITAL_UP)) return true;
  }

  return false;
}