#include "config.hpp"
#include "Libraries/controller.hpp"

_Controller master (CONTROLLER_MASTER);
_Controller partner(CONTROLLER_PARTNER);

static constexpr int e_port = 20;

pros::ADIEncoder LeftEncoder(1, 2, false), RightEncoder(5, 6, true), BackEncoder(3, 4, false); //remove

// tank-drive motor config
pros::Rotation left_tracker(1), right_tracker(2), back_tracker(3);
pros::Motor front_l(15, MOTOR_GEARSET_18, true, MOTOR_ENCODER_DEGREES);
pros::Motor centre_l(14, MOTOR_GEARSET_18, false, MOTOR_ENCODER_DEGREES);
pros::Motor back_l(16, MOTOR_GEARSET_18, true, MOTOR_ENCODER_DEGREES);

pros::Motor front_r(11, MOTOR_GEARSET_18, false, MOTOR_ENCODER_DEGREES);
pros::Motor centre_r(12, MOTOR_GEARSET_18, true, MOTOR_ENCODER_DEGREES);
pros::Motor back_r(17, MOTOR_GEARSET_18, false, MOTOR_ENCODER_DEGREES);

pros::Motor b_lift_m(16);
