#pragma once
#include "util.hpp"


class PID {
public:
    double kP, kI, kD;
    double proporational = 0, integral = 0, derivative = 0;
    double lower_limit, upper_limit;
    int last_D = 0, last_I = 0;
    double last_error = 0;
    int sgnLast;


    PID(double kP, double kI, double kD, double lower_limit, double upper_limit):
    kP(kP),
    kI(kI),
    kD(kD),
    lower_limit(lower_limit),
    upper_limit(upper_limit)
    {}

    double calculate(double error);

};