#pragma once
#include "main.h"

class Button;
class Text_;

//maybe a get_port if needed?
class _Motor : public Motor{
  private:    
    // bool state;  // the state of the SUBSYSTEM (e.g. 1 if the claw is closed, 0 if open)
    // bool reversed;  // if the state of the subsytem is opposite the state of the piston
    int change_time = 0;
    std::string name;

    static int count;
  public:
    static std::array<std::tuple<_Motor*, Button*, Text_*>, 8> list_for_gui;
    //this, toggle button, temperature text

    _Motor(std::int8_t port, std::string name, bool reversed = false, motor_gearset_e_t gearset = MOTOR_GEAR_BLUE, motor_encoder_units_e_t encoder_units = MOTOR_ENCODER_DEGREES);

    void move(int voltage);
    void move_relative(int velocity);

    int getStateTime() const;
    std::string getName() const;
    std::string getShortName() const;

    //static overloads (mostly for Nathan)
    static std::string get_name(int number);
};