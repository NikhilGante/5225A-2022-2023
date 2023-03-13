#pragma once
#include "../util.hpp"
#include "../Libraries/gui.hpp"
#include "../config.hpp"
#include "pros/apix.h"

class _Motor: private Motor, public ObjectTracker<_Motor, 8>{
  private:
    Button on, off;
    Text<int> temperature, data;
    Text<> text;
    int speed; //Represents the target speed

    int velocityToVoltage(int velocity);

  public:
    _Motor(std::int8_t port, std::string name, bool reversed = false, motor_gearset_e_t gearset = MOTOR_GEAR_BLUE, motor_encoder_units_e_t encoder_units = MOTOR_ENCODER_DEGREES);
    inline static const c::v5_device_e device = c::E_DEVICE_MOTOR;

    void move(int voltage);
    void moveRelative(int velocity);
    void brake();

    void updateTemperatureText();

    bool plugged() const;
    double getPosition() const;
    double getTargetPosition() const;
    int getTemperature() const;
    int getRPM() const;
    Port getPort() const;
};