#include "config.hpp"
#include "Libraries/controller.hpp"
#include "Libraries/piston.hpp"

_Controller master (CONTROLLER_MASTER);
_Controller partner(CONTROLLER_PARTNER);

static constexpr int e_port = 20;

ADIEncoder LeftEncoder(1, 2, false), RightEncoder(5, 6, true), BackEncoder(3, 4, false);

// tank-drive motor config
Rotation left_tracker(6), right_tracker(1), back_tracker(10);
Motor front_l(20, E_MOTOR_GEARSET_06, false, E_MOTOR_ENCODER_DEGREES);
Motor centre_l(19, E_MOTOR_GEARSET_06, true, E_MOTOR_ENCODER_DEGREES);
Motor back_l(17, E_MOTOR_GEARSET_06, false, E_MOTOR_ENCODER_DEGREES);

Motor front_r(11, E_MOTOR_GEARSET_06, true, E_MOTOR_ENCODER_DEGREES);
Motor centre_r(12, E_MOTOR_GEARSET_06, false, E_MOTOR_ENCODER_DEGREES);
Motor back_r(13, E_MOTOR_GEARSET_06, true, E_MOTOR_ENCODER_DEGREES);

Motor intake_m(5, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES);
Motor flywheel_m(9, E_MOTOR_GEARSET_06, false, E_MOTOR_ENCODER_DEGREES);

Piston indexer_p('E', "indexer_p", false, LOW);

ADIAnalogIn mag_ds('A');

Rotation flywheel_rot_sensor(3);	// Configures rotation sensor in port 1

Piston angler_p('H', "angler_p", true, LOW);

Motor b_lift_m(16);
