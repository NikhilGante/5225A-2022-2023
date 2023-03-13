#include "controller.hpp"
#include "../Libraries/task.hpp"
#include "../Libraries/timer.hpp"
#include "../util.hpp"

_Task _Controller::controller_task{"Controller"};
_Controller* _Controller::master_ptr{nullptr};
_Controller* _Controller::partner_ptr{nullptr};
std::string _Controller::line_0{};
std::string _Controller::line_1{};
std::string _Controller::line_2{};

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
  queue.name = name + " Controller";
}

void _Controller::handle(){
  if(!queue.empty()){
    state_log("%d: Running function on %s controller\n", millis(), name);
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
    state_log("%d: clearing line %d on %s controller", millis(), line, name);
  });
  getText(line) = "";
  state_log("%d: Adding clearLine for %s controller", millis(), name);
}

void _Controller::clear(){
  queue.push([=, this](){
    Controller::clear();
    state_log("%d: Clearing screen on %s controller", millis(), name);
  });
  getText(0) = getText(1) = getText(2) = "";
  state_log("%d: Adding clear to %s controller queue", millis(), name);
}

void _Controller::rumble(std::string rumble_pattern){
  queue.push([=, this](){
    Controller::rumble(rumble_pattern.c_str());
    state_log("%d: Rumbling %s controller", millis(), name);
  });
  state_log("%d: Adding rumble to %s controller queue", millis(), name);
}

controller_digital_e_t _Controller::waitForPress(std::vector<controller_digital_e_t> buttons, int timeout){
  Timer timer{"Controller Button Timeout", state_log};
  state_log("%d: Waiting for button press on %s controller with a timeout of %d", millis(), name, timeout);
  
  WAIT_UNTIL(timer.getTime() > timeout){
    for(controller_digital_e_t btn: buttons){
      if(getNewDigital(btn)){
        state_log("%d: Button %d pressed on %s controller", millis(), btn, name);
        return btn;
      }
    }
  }
  state_log("%d: Timed out on waiting for button press from controller %s", millis(), name);
  return static_cast<controller_digital_e_t>(0);
}

void _Controller::waitForPress(controller_digital_e_t button, int timeout) {waitForPress(std::vector{button}, timeout);}
bool _Controller::getDigital(controller_digital_e_t button) {return get_digital(button);}
bool _Controller::getNewDigital(controller_digital_e_t button) {return get_digital_new_press(button);}
bool _Controller::connected() {return is_connected();}
int _Controller::getAnalog(controller_analog_e_t joystick, int deadzone) {return deadband(get_analog(joystick));}

_Controller* _Controller::id_to_ptr(controller_id_e_t id){
  switch(id){
    case CONTROLLER_MASTER: return master_ptr; break;
    case CONTROLLER_PARTNER: return partner_ptr; break;
  }
  return nullptr;
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

void _Controller::blackout(){
  print(0, "22222222222222222222");
  print(1, "22222222222222222222");
  print(2, "22222222222222222222");
}