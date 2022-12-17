#pragma once
#include "main.h"
#include "gui.hpp"
#include "counter.hpp"

class _Motor: protected Motor, public Counter<_Motor, 8>{
  private:
    Button on, off;
    Text<int> temperature, data;
    Text<> text;
    std::string name, short_name;

    int speed;

  public:
    _Motor(std::int8_t port, std::string name, bool reversed = false, motor_gearset_e_t gearset = MOTOR_GEAR_BLUE, motor_encoder_units_e_t encoder_units = MOTOR_ENCODER_DEGREES);

    void move(int voltage);
    void brake();
    void updateTemperatureText();

    double getPosition() const;
    int getTemperature() const;
    int getRPM() const;
    std::string getName() const;
    std::string getShortName() const;
};