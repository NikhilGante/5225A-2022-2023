#pragma once
#include "main.h"
#include "logging.hpp"
#include "queue.hpp"

// Buttons
  constexpr controller_digital_e_t okBtn = DIGITAL_A;
  
  // ------------------------------ Direction Buttons ------------------------------
  extern controller_digital_e_t goalDisturbBtn;
  // Up and Down Taken by Adjusting Disc count


  // ------------------------------ Letter Buttons ------------------------------
  extern controller_digital_e_t intakeToggleBtn;
  extern controller_digital_e_t intakeRevBtn;
  extern controller_digital_e_t angleOverrideBtn;
  extern controller_digital_e_t endgameBtn;


  // ------------------------------ Front Buttons ------------------------------
  extern controller_digital_e_t anglerToggleBtn;
  extern controller_digital_e_t transToggleBtn;
  extern controller_digital_e_t singleShotBtn;
  extern controller_digital_e_t tripleShotBtn;

  // partner buttons

class _Task;

class _Controller: public pros::Controller{
  private:
    static _Controller *master_ptr, *partner_ptr;
    static _Task controller_task;
    static Queue<std::function<void()>, 20> queue;
    std::string name;

  public:
    _Controller(pros::controller_id_e_t id);
    static void init();

    static constexpr int deadzone = 7;
    
    void handle();
    void clearLine (std::uint8_t line);
    void clear();
    void rumble(std::string rumble_pattern);
    int getAnalog(controller_analog_e_t joystick);
    int getAnalog(controller_analog_e_t joystick, int deadzone);
    bool interrupt(bool analog = true, bool digital = true, bool OK_except = true);
    void wait_for_press(controller_digital_e_t button, int timeout = std::numeric_limits<int>::max());
    controller_digital_e_t wait_for_press(std::vector<controller_digital_e_t> buttons, int timeout = std::numeric_limits<int>::max());

    void print(std::uint8_t line, std::uint8_t col, std::string fmt, auto... args){
      std::string str = sprintf2(fmt, args...);
      queue.push([=, this](){
        pros::Controller::print(line, col, str.c_str());
        controller_data.print("Printing %s to %s controller", str, name);
      });
      controller_data.print("Adding print to %s controller queue", name);
    }
};