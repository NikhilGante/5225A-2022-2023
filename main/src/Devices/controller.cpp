#include "controller.hpp"
#include "../Libraries/task.hpp"
#include "../Libraries/timer.hpp"
#include "../util.hpp"

_Task _Controller::controller_task{"Controller"};
_Controller* _Controller::master_ptr{nullptr};
_Controller* _Controller::partner_ptr{nullptr};
std::string _Controller::line_0{"Line 0 Empty"};
std::string _Controller::line_1{"Line 1 Empty"};
std::string _Controller::line_2{"Line 2 Empty"};

_Controller::_Controller(controller_id_e_t id): Controller{id}{
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
    Controller::clear_line(line);
    controller_log("clearing line %d on %s controller", line, name);
  });
  controller_log("Adding clearLine for %s controller", name);
}

void _Controller::clear(){
  queue.push([=, this](){
    Controller::clear();
    controller_log("Clearing screen on %s controller", name);
  });
  controller_log("Adding clear to %s controller queue", name);
}

void _Controller::rumble(std::string rumble_pattern){
  queue.push([=, this](){
    Controller::rumble(rumble_pattern.c_str());
    controller_log("Rumbling %s controller", name);
  });
  controller_log("Adding rumble to %s controller queue", name);
}

controller_digital_e_t _Controller::wait_for_press(std::vector<controller_digital_e_t> buttons, int timeout){
  Timer timer{"Controller Button Timeout", controller_log};
  controller_log("Waiting for button press on %s controller with a timeout of %d", name, timeout);
  
  WAIT_UNTIL(timer.getTime() > timeout){
    for(controller_digital_e_t btn: buttons){
      if(getNewDigital(btn)){
        controller_log("Button %d pressed on %s controller", btn, name);
        return btn;
      }
    }
  }
  controller_log("Timed out on waiting for button press from controller %s", name);
  return static_cast<controller_digital_e_t>(0);
}

void _Controller::wait_for_press(controller_digital_e_t button, int timeout) {wait_for_press(std::vector{button}, timeout);}

bool _Controller::getDigital(controller_digital_e_t button) {return get_digital(button);}
bool _Controller::getNewDigital(controller_digital_e_t button) {return get_digital_new_press(button);}

int _Controller::getAnalog(controller_analog_e_t joystick, int deadzone){
  int value = get_analog(joystick);
  return std::abs(value) > deadzone ? value : 0;
}

bool _Controller::interrupt(bool analog, bool digital, bool OK_except){
  if (analog){
    if (getAnalog(ANALOG_LEFT_X ), 20) return true;
    if (getAnalog(ANALOG_LEFT_Y ), 20) return true;
    if (getAnalog(ANALOG_RIGHT_X), 20) return true;
    if (getAnalog(ANALOG_RIGHT_Y), 20) return true;
  }
  if(digital){
    if (getDigital(okBtn)) return !OK_except;
    if (getDigital(DIGITAL_A)) return true;
    if (getDigital(DIGITAL_B)) return true;
    if (getDigital(DIGITAL_Y)) return true;
    if (getDigital(DIGITAL_X)) return true;
    if (getDigital(DIGITAL_R1)) return true;
    if (getDigital(DIGITAL_R2)) return true;
    if (getDigital(DIGITAL_L1)) return true;
    if (getDigital(DIGITAL_L2)) return true;
    if (getDigital(DIGITAL_RIGHT)) return true;
    if (getDigital(DIGITAL_DOWN)) return true;
    if (getDigital(DIGITAL_LEFT)) return true;
    if (getDigital(DIGITAL_UP)) return true;
  }

  return false;
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