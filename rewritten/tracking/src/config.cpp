#include "config.hpp"


pros::Controller master (pros::E_CONTROLLER_MASTER);

// static constexpr int e_port = 20;

// Motors and Encoders
pros::ADIEncoder LeftEncoder(1, 2, true);
pros::ADIEncoder RightEncoder(3, 4, true);
pros::ADIEncoder BackEncoder(5, 6, false);

// X-Drive Motors
pros::Motor front_l(3, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_l(2, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor front_r(7, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_r(1, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);