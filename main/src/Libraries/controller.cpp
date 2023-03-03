#include "controller.hpp"

// Buttons


// ------------------------------ Direction Buttons ------------------------------
controller_digital_e_t goalDisturbBtn = DIGITAL_LEFT;
controller_digital_e_t endgameBtnLeft = DIGITAL_RIGHT;

// Up and Down Taken by Adjusting Disc count



// ------------------------------ Letter Buttons ------------------------------
controller_digital_e_t intakeToggleBtn = DIGITAL_A;
controller_digital_e_t intakeRevBtn = DIGITAL_X;
controller_digital_e_t endgameBtnRight = DIGITAL_Y;


// ------------------------------ Front Buttons ------------------------------
controller_digital_e_t anglerToggleBtn = DIGITAL_UP;
controller_digital_e_t transToggleBtn = DIGITAL_L2;
controller_digital_e_t singleShotBtn = DIGITAL_R1;
controller_digital_e_t tripleShotBtn = DIGITAL_R2;

// Controller queue stuff

std::array<_Controller*, num_controller> _Controller::objs; //= {nullptr};
_Task _Controller::controller_task("controller task");

int constructed = 0;

_Controller::_Controller(pros::controller_id_e_t id): pros::Controller{id}
{
  objs[constructed] = this;
  this->controller_num = constructed+1;
  constructed++;
}

void _Controller::printQueue(){
  while(true){
    for(int i = 0; i < num_controller; i++){
      objs[i]->queueHandle();
      _Task::delay(50);
    }
  }
}

void _Controller::init(){
  controller_task.start(printQueue);
}

void _Controller::queueHandle(){
  if(!queue.isEmpty()){
    // printf("running command on controller %d", controller_num);
    queue.pop()();  // run the next function
  }
}

//template this at some point (done in header_clean)
void _Controller::print(std::uint8_t line, std::uint8_t col, const char* fmt, ... ){
  char buffer[19];
  std::va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, 19, fmt, args);
  va_end(args);
  string buffer_cpy = buffer;
  std::function<void()> func = [&, line, col, buffer_cpy](){
    pros::Controller::print(line, col, buffer_cpy.c_str());
    // printf("printing %s to %d", buffer_cpy.c_str(), this->controller_num);
  };
  queue.push(func);
  // printf("adding print to queue for controller %d", this->controller_num);
}
void _Controller::print(std::uint8_t line, std::uint8_t col, std::string str){
  std::function<void()> func = [&, line, col, str](){
    pros::Controller::print(line, col, str.c_str());
    printf("printing %s to %d\n", str.c_str(), this->controller_num);
  };
  queue.push(func);
  printf("adding print to queue for controller %d\n", this->controller_num);
}

void _Controller::clear_line (std::uint8_t line){
  std::function<void()> func = [&, line](){
    pros::Controller::clear_line(line);
    printf("clearing line %d for controller %d", line, this->controller_num);
  };
  queue.push(func);
  printf("adding clear_line to queue for controller %d\n", this->controller_num);
}

void _Controller::clear(){
  std::function<void()> func = [&](){
    pros::Controller::clear();
    printf("clearing %d", this->controller_num);
  };
  queue.push(func);
  printf("adding clear to queue for controller %d\n", this->controller_num);
}


void _Controller::rumble(const string& rumble_pattern){
  std::function<void()> func = [&, rumble_pattern](){
    pros::Controller::rumble(rumble_pattern.c_str());
    printf("rumble controller %d", this->controller_num);
  };
  queue.push(func);
  printf("adding rumble to queue for controller %d\n", this->controller_num);
}


controller_digital_e_t _Controller::waitForPress(std::vector<controller_digital_e_t> buttons, int timeout){
  int start_time = millis();
  printf("waiting for button press from controller %d\n", this->controller_num);
  controller_digital_e_t button = static_cast<controller_digital_e_t>(0);
  
  WAIT_UNTIL(button != static_cast<controller_digital_e_t>(0)){
    for(std::vector<controller_digital_e_t>::const_iterator it = buttons.begin(); it != buttons.end(); it++){
      if(get_digital_new_press(*it)) button = *it;
    }

    if(timeout != 0 && millis() - start_time > timeout){
      printf("timed out on waiting for button press from controller %d\n", this->controller_num);
      return static_cast<controller_digital_e_t>(0);
    }
  }
  printf("button %d pressed from controller %d\n", button, this->controller_num);
// button handling methods

  return button;
}


//create wait for press for multiple buttons and return the one that was pressed
void _Controller::waitForPress(controller_digital_e_t button, int timeout){
  int start_time = millis();
  printf("waiting for button %d from controller %d", button, this->controller_num);
  
  WAIT_UNTIL(get_digital_new_press(button)){
    if(timeout != 0 && millis() - start_time > timeout){
      printf("timed out on waiting for button %d press from controller %d", button, this->controller_num);
      return;
    }
  }
  printf("button %d pressed from controller %d", button, this->controller_num);
}

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