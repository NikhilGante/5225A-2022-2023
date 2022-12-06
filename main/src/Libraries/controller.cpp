#include "controller.hpp"
#include "task.hpp"
#include "timer.hpp"

// Buttons

controller_digital_e_t endgameBtn1 = DIGITAL_RIGHT;
// controller_digital_e_t someBUTTON = DIGITAL_DOWN;
controller_digital_e_t resetDiscCountBtn = DIGITAL_LEFT;
// controller_digital_e_t someBUTTON = DIGITAL_UP;

controller_digital_e_t intakeToggleBtn = DIGITAL_A;
controller_digital_e_t rollerBtn = DIGITAL_B;
controller_digital_e_t endgameBtn2 = DIGITAL_Y;
controller_digital_e_t transToggleBtn = DIGITAL_X;

controller_digital_e_t anglerToggleBtn = DIGITAL_L1;
// controller_digital_e_t someBtn = DIGITAL_L2;
controller_digital_e_t tripleShotBtn = DIGITAL_R2;
controller_digital_e_t singleShotBtn = DIGITAL_R1;

_Task _Controller::controller_task{"controller task"};
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
}

void _Controller::queueHandle(){
  if(!queue.empty()){
    controller_queue.print("Running function on %s controller\n", name);
    queue.front()();
    queue.pop();
    _Task::delay(50);
  }
}

void _Controller::init(){
  controller_task.start([](){
    while(true){
      if(master_ptr) master_ptr->queueHandle();
      if(partner_ptr) partner_ptr->queueHandle();
      _Task::delay();
    }
  });
}


void _Controller::clear_line (std::uint8_t line){
  queue.push([=, this](){
    pros::Controller::clear_line(line);
    controller_queue.print("clearing line %d on %s controller", line, name);
  });
  controller_queue.print("Adding clear_line for %s controller", name);
}

void _Controller::clear(){
  queue.push([=, this](){
    pros::Controller::clear();
    controller_queue.print("Clearing screen on %s controller", name);
  });
  controller_queue.print("Adding clear to %s controller queue", name);
}


void _Controller::rumble(std::string rumble_pattern){
  queue.push([=, this](){
    pros::Controller::rumble(rumble_pattern.c_str());
    controller_queue.print("Rumbling %s controller", name);
  });
  controller_queue.print("Adding rumble to %s controller queue", name);
}

controller_digital_e_t _Controller::wait_for_press(std::vector<controller_digital_e_t> buttons, int timeout){
  Timer timer{"Controller Button Timeout"};
  controller_queue.print("Waiting for button press on %s controller with a timeout of %d", name, timeout);
  
  WAIT_UNTIL(timer.getTime() > timeout){
    for(controller_digital_e_t btn: buttons){
      if(get_digital_new_press(btn)){
        controller_queue.print("Button %d pressed on %s controller", btn, name);
        return btn;
      }
    }
  }
  controller_queue.print("Timed out on waiting for button press from controller %s", name);
  return static_cast<controller_digital_e_t>(0);
}

void _Controller::wait_for_press(controller_digital_e_t button, int timeout) {wait_for_press(std::vector{button}, timeout);}

void _Controller::updateButtons(){
  for(int i = 0; i < 12; i++){
    last_press_arr[i] = cur_press_arr[i];
    // + 6 because controller_digital_e_t starts with 6 instead of 0
    cur_press_arr[i] = this->get_digital(static_cast<pros::controller_digital_e_t>(i + 6));
    // printf2("%d, %d ", i, cur_press_arr[i]);
  }
}

bool _Controller::getButtonState(pros::controller_digital_e_t button){
  return cur_press_arr[static_cast<int>(button) - 6];
}

bool _Controller::getButtonLastState(pros::controller_digital_e_t button){
  return last_press_arr[static_cast<int>(button) - 6];
}

bool _Controller::isRising(pros::controller_digital_e_t button){
  int index = static_cast<int>(button) - 6;
  return !last_press_arr[index] && cur_press_arr[index];
}

bool _Controller::isFalling(pros::controller_digital_e_t button){
  int index = static_cast<int>(button) - 6;
  return last_press_arr[index] && !cur_press_arr[index];
}