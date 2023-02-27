#pragma once
#include "main.h"
#include "../Libraries/logging.hpp"
#include "../util.hpp"

// Buttons
  static constexpr controller_digital_e_t okBtn = DIGITAL_A;

  // ------------------------------ Direction Buttons ------------------------------
  static constexpr controller_digital_e_t goalDisturbBtn = DIGITAL_LEFT;
  static constexpr controller_digital_e_t endgameBtnLeft = DIGITAL_RIGHT;

  // ------------------------------ Letter Buttons ------------------------------
  static constexpr controller_digital_e_t intakeToggleBtn = DIGITAL_A;
  static constexpr controller_digital_e_t intakeRevBtn = DIGITAL_X;
  static constexpr controller_digital_e_t angleOverrideBtn = DIGITAL_B;
  static constexpr controller_digital_e_t endgameBtnRight = DIGITAL_Y;


  // ------------------------------ Front Buttons ------------------------------
  static constexpr controller_digital_e_t anglerToggleBtn = DIGITAL_L1;
  static constexpr controller_digital_e_t transToggleBtn = DIGITAL_L2;
  static constexpr controller_digital_e_t singleShotBtn = DIGITAL_R1;
  static constexpr controller_digital_e_t tripleShotBtn = DIGITAL_R2;

class _Task;

class _Controller: private Controller{
  private:
    static _Controller *master_ptr, *partner_ptr;
    static _Task controller_task;
    static std::string line_0, line_1, line_2;

    Queue<std::function<void()>, 20> queue;
    std::string name;

    void handle();
    std::string& getText(int line);

    _Controller(_Controller const &) = delete;
    _Controller& operator=(_Controller const &) = delete;
    
  public:
    _Controller(controller_id_e_t id);
    static void init();

    static constexpr int deadzone = 7;

    static void deadband(auto& value) {value = okapi::deadband(value, -deadzone, deadzone);}
    static auto deadband(auto&& value) {return okapi::deadband(value, -deadzone, deadzone);}
    
    void clearLine (std::uint8_t line);
    void clear();
    void rumble(std::string rumble_pattern = "-");
    int  getAnalog(controller_analog_e_t joystick, int deadzone = _Controller::deadzone);
    bool getDigital(controller_digital_e_t);
    bool getNewDigital(controller_digital_e_t);
    bool connected();
    bool interrupt(bool analog = true, bool digital = true, bool OK_except = true);
    void waitForPress(controller_digital_e_t button, int timeout = std::numeric_limits<int>::max());
    controller_digital_e_t waitForPress(std::vector<controller_digital_e_t> buttons, int timeout = std::numeric_limits<int>::max());
    std::string getText(int line) const;

    void print(std::uint8_t line, std::string fmt, auto... args){
      clearLine(line);
      std::string str = sprintf2(fmt, args...);
      queue.push([=, this](){
        Controller::print(line, 0, str.c_str());
        controller_log("Printing \"%s\" to %s controller", str, name);
      });
      controller_log("Adding print to %s controller queue", name);

      getText(line) = str;
    }

    void printScroll(std::string fmt, auto... args){
      std::string str = sprintf2(fmt, args...);
      if (getText(2) == ""){
        print(2, str);
      }
      else if (getText(1) == ""){
        print(1, getText(2));
        print(2, str);
      }
      else{
        print(0, getText(1));
        print(1, getText(2));
        print(2, str);
      }
    }
};