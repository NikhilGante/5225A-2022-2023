#pragma once
#include "../util.hpp"
#include "timer.hpp"

class Logging;

class PID{
  private:
    Timer last_update_timer;
    bool integral_sgn_reset; // if the integral should when the sgn fo the error flips
    uint8_t last_error_sgn;   // used if user wants to reset the integral after the sgn changes
    double error, last_error, kP, kI, kD, proportional, integral, integral_lower_bound, integral_upper_bound, derivative, bias, output;

  public:
    PID(std::string name, Logging& log, double kP, double kI, double kD, double bias, bool integralSgnReset = true, double integralLowerBound = 0, double integralUpperBound = std::numeric_limits<double>::max());
    double getError() const;
    double getOutput() const;
    double getProportional() const;
    double compute(double input, double target);
};