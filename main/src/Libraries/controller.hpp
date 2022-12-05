#pragma once
#include "main.h"
#include "logging.hpp"
#include "queue.hpp"
#include <vector>
#include <cstdarg>
#include <array>
#include <functional>

using namespace pros;
using namespace std;

// Buttons

// partner buttons

class _Task;

class _Controller : public pros::Controller{
private:
  static constexpr int count = 1;
  static _Controller *master_ptr, *partner_ptr;
  static _Task_ controller_task;
  Queue<std::function<void()>, 20> queue{"Controller"};
  std::string name;

  // button handling data
  bool cur_press_arr[12] = {0};
  bool last_press_arr[12] = {0};

  void queue_handle();

public:
  _Controller(pros::controller_id_e_t id);
  static void init();

  void clear_line (std::uint8_t line);
  void clear();
  void rumble(std::string rumble_pattern);
  void wait_for_press(controller_digital_e_t button, int timeout = 0);
  controller_digital_e_t wait_for_press(std::vector<controller_digital_e_t> buttons, int timeout = std::numeric_limits<int>::max());

  template <typename... Params>
  void print(std::uint8_t line, std::uint8_t col, std::string fmt, Params... args){
    std::string str = sprintf2(fmt, args...);
    queue.push([=, this](){
      pros::Controller::print(line, col, str.c_str());
      controller_queue.print("Printing %s to %s controller", str.c_str(), name);
    });
    controller_queue.print("Adding print to %s controller queue", name);
  }

  // button handling methods
  // NOTE: all the following methods are only updated every cycle as opposed to every function call, unlike the pros API

  void updateButtons();  // called once every loop, updates current and last state for every button
  bool getButtonState(pros::controller_digital_e_t button); // returns current state of desired button
  bool getButtonLastState(pros::controller_digital_e_t button); // returns last state of desired button
  bool isRising(pros::controller_digital_e_t button); // if button wasn't pressed but now is
  bool isFalling(pros::controller_digital_e_t button); // if button was pressed but now is not
};