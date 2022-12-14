#include "pid.hpp"
#include "../util.hpp"

PID::PID(double kP, double kI, double kD, double bias, bool integral_sgn_reset, double integral_lower_bound, double integral_upper_bound):
  kP(kP), kI(kI), kD(kD),
  bias(bias),
  integral_sgn_reset(integral_sgn_reset),
  integral_upper_bound(integral_upper_bound), integral_lower_bound(integral_lower_bound)
{}

double PID::getError() const {
  return error;
}

double PID::getOutput() const {
  return output;
}

double PID::getProportional() const{
  return proportional;
}

double PID::compute(double input, double target){
  error = target - input;

  proportional = error * kP;
  //(kI || kD) && 
  if (last_update_timer.getTime() > 0.0){  // if only P is activated, don't compute I and D-related variables
    // resets integral if sign of error flips and user enabled this feature, or error is out of bounds
    if ((integral_sgn_reset && sgn(error) != last_error_sgn) || (std::abs(error) < integral_lower_bound || std::abs(error) > integral_upper_bound)){
      integral = 0;
    }
    else integral += error * last_update_timer.getTime() * kI;

    derivative = (error - last_error) / last_update_timer.getTime() * kD;

    // updates variables for next iteration of loop
    last_update_timer.reset();
    last_error_sgn = sgn(error);
    last_error = error;
    // motion_d.print("calculating derivative : %f", derivative);
  }
    
  output = proportional + integral + derivative + bias;

  // motion_i.print(" P: %f, D:%f, In:%f, Out:%f", proportional, derivative, input, output);
  return output;
}