#pragma once
#include "main.h"
#include "timer.hpp"
#include "task.hpp"
#include "../util.hpp"
#include "queue.hpp"
#include <vector>
#include <cstdarg>
#include <array>
#include <functional>

using namespace pros;
using namespace std;

// Buttons
// ------------------------------ Direction Buttons ------------------------------
extern controller_digital_e_t flywheelToggleBtn;
extern controller_digital_e_t endgameBtnLeft;
// Up and Down Taken by Adjusting Disc count






// ------------------------------ Letter Buttons ------------------------------
extern controller_digital_e_t intakeToggleBtn;
extern controller_digital_e_t intakeRevBtn;
extern controller_digital_e_t endgameBtnRight;
extern controller_digital_e_t fast_turn_btn;


// ------------------------------ Front Buttons ------------------------------
extern controller_digital_e_t anglerToggleBtn;
extern controller_digital_e_t transToggleBtn;
extern controller_digital_e_t ShotBtn;
extern controller_digital_e_t rollerBtn;



// partner buttons

#define num_controller 2

class _Task;

class _Controller : public pros::Controller{
private:
  // std::array<std::function<void()>,20> queue;
  static std::array<_Controller*, num_controller> objs;
  int controller_num;
  std::string line_0, line_1, line_2;

  // button handling data
  bool cur_press_arr[12] = {0};
  bool last_press_arr[12] = {0};

public:
  void queueHandle();

  Queue<std::function<void()>, 20> queue{"controller_queue"};

  _Controller(pros::controller_id_e_t id);
  static void printQueue();
  static _Task controller_task;
  static void init();

  void clear_line (std::uint8_t line);
  void clear();
  void rumble(const string & rumble_pattern);
  void waitForPress(controller_digital_e_t button, int timeout = 0);
  /**
   * @brief Waits for any button from param buttons to be pressed
   * 
   * @param buttons list of buttons to check
   * @param timeout how long to wait before timing out on the wait
   * @return the button that was pressed. 0 if nothing pressed
   */
  controller_digital_e_t waitForPress(std::vector<controller_digital_e_t> buttons, int timeout = 0);

  void print(std::uint8_t line, std::uint8_t col, std::string fmt, auto... args){
    char buffer[19];
    snprintf(buffer, 19, fmt.c_str(), args...);
    std::string str = buffer;
    str += std::string(19-str.length(), ' ');
    queue.push([=, this](){
      Controller::print(line, col, str.c_str());
      // printf("printing %s to %d", str, this->controller_num);
    });
    // printf("adding print to queue for controller %d", this->controller_num);

    switch(line){
      case 0: line_0 = str; break;
      case 1: line_1 = str; break;
      case 2: line_2 = str; break;
    }
  }

  void printScroll(std::string fmt, auto... args){
    char buffer[19];
    snprintf(buffer, 19, fmt.c_str(), args...);
    std::string str = buffer;
    // if (line_2 != ""){
    //   if (line_1 != "") print(0, 0, line_1);
    //   print(1, 0, line_2);
    // }
    print(0, 0, line_1);
    print(1, 0, line_2);
    print(2, 0, str);
  }

  // button handling methods
  // NOTE: all the following methods are only updated every cycle as opposed to every function call, unlike the pros API

  void updateButtons();  // called once every loop, updates current and last state for every button
  bool getButtonState(pros::controller_digital_e_t button); // returns current state of desired button
  bool getButtonLastState(pros::controller_digital_e_t button); // returns last state of desired button
  bool isRising(pros::controller_digital_e_t button); // if button wasn't pressed but now is
  bool isFalling(pros::controller_digital_e_t button); // if button was pressed but now is not

};