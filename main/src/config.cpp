#include "config.hpp"

_Controller master(pros::E_CONTROLLER_MASTER);
_Controller partner(pros::E_CONTROLLER_PARTNER);

pros::Motor front_l(1, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor center_l(9, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_l(5, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);

pros::Motor front_r(6, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor center_r(10, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_r(3, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);

pros::Motor flywheel_front(1, pros::E_MOTOR_GEARSET_06, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor flywheel_back(2, pros::E_MOTOR_GEARSET_06, false, pros::E_MOTOR_ENCODER_DEGREES);


pros::Motor f_lift_m(4, pros::E_MOTOR_GEARSET_18, false,pros::E_MOTOR_ENCODER_DEGREES); 
pros::Motor b_lift_m(16, pros::E_MOTOR_GEARSET_36, false,pros::E_MOTOR_ENCODER_DEGREES);
pros::ADIAnalogIn f_lift_pot(8), b_lift_pot(2);