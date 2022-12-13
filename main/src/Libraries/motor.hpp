#pragma once
#include "main.h"
#include "gui.hpp"

class _Motor : public Motor{
  private:
    Button on, off;
    Text<int> temperature;
    Text<> text;
    std::string name, short_name;
    static int count;

    int speed;

  public:
    static std::array<_Motor*, 8> list_for_gui;

    _Motor(std::int8_t port, std::string name, bool reversed = false, motor_gearset_e_t gearset = MOTOR_GEAR_BLUE, motor_encoder_units_e_t encoder_units = MOTOR_ENCODER_DEGREES);

    void move(int voltage);
    void brake();
    void updateTemperatureText();

    //Getters
      int getTemperature() const;
      std::string getName() const;
      std::string getShortName() const;
};