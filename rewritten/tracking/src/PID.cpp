#include "PID.hpp"


double PID::calculate(double error){

    proporational = error*kP;
    

    if ((fabs(error)<lower_limit) || (fabs(error)>upper_limit) || sgn(error) != sgnLast) integral = 0;
    else integral += kI*(error/(pros::millis()-last_I));
    last_I = pros::millis();



    // std::cout << error << ' ' << last_error << ' ' << last_D << ' ' << pros::millis() << std::endl;
    
    derivative = (error-last_error)/(pros::millis()-last_D)*kD;
    last_D = pros::millis();



    last_error = error;
    sgnLast = sgn(error);
    // std::cout << proporational << ' ' <<  integral << ' ' <<  derivative << std::endl;
    return proporational+integral+derivative;
}