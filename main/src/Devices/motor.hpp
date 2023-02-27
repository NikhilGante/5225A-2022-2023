#pragma once
#include "main.h"
#include "../Libraries/gui.hpp"

class _Motor: private Motor, public ObjectTracker<_Motor, 8>{
  private:
    Button on, off;
    Text<int> temperature, data;
    Text<> text;
    std::string name, short_name;
    int speed; //Represents the target speed

    int velocityToVoltage(int velocity);

  public:
    _Motor(std::int8_t port, std::string name, bool reversed = false, motor_gearset_e_t gearset = MOTOR_GEAR_BLUE, motor_encoder_units_e_t encoder_units = MOTOR_ENCODER_DEGREES);

    void move(int voltage);
    void moveRelative(int velocity);
    void brake();

    void updateTemperatureText();

    bool plugged() const;
    double getPosition() const;
    double getTargetPosition() const;
    int getTemperature() const;
    int getRPM() const;
    std::string getName() const;
    std::string getShortName() const;
};