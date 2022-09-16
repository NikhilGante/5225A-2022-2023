#include "config.hpp"


Controller master(pros::E_CONTROLLER_MASTER);
Controller partner(pros::E_CONTROLLER_PARTNER);

static constexpr int e_port = 20;

pros::ADIEncoder LeftEncoder(1, 2, false), RightEncoder(5, 6, true), BackEncoder(3, 4, false);

// tank-drive motor config
pros::Rotation left_tracker(1), right_tracker(2), back_tracker(3);
pros::Motor front_l(15, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor centre_l(14, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_l(16, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);

pros::Motor front_r(11, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor centre_r(12, pros::E_MOTOR_GEARSET_18, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor back_r(17, pros::E_MOTOR_GEARSET_18, false, pros::E_MOTOR_ENCODER_DEGREES);

pros::Motor b_lift_m(16);
