#include "config.hpp"

_Controller master(pros::E_CONTROLLER_MASTER);
_Controller partner(pros::E_CONTROLLER_PARTNER);

static constexpr int e_port = 20;

pros::ADIEncoder LeftEncoder({e_port, 1, 2}, false), RightEncoder({e_port, 5, 6}, true), BackEncoder({e_port, 3, 4}, false);

// x-drive motor config
// pros::Motor front_l(3, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor back_l(2, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor front_r(7, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
// pros::Motor back_r(1, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);

pros::Motor flywheel_m(8, pros::E_MOTOR_GEARSET_06, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::ADIAnalogIn disc_sensor('G');
Piston indexer_p('A', "indexer p", 0, 0);