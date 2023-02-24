#include "config.hpp"


_Controller master(E_CONTROLLER_MASTER);
_Controller partner(E_CONTROLLER_PARTNER);

static constexpr int e_port = 20;

// ADIEncoder LeftEncoder(1, 2, false), RightEncoder(5, 6, true), BackEncoder(3, 4, false);

// tank-drive motor config
Rotation left_tracker(3), right_tracker(4), back_tracker(5);




Motor front_l(11, E_MOTOR_GEARSET_06, false, E_MOTOR_ENCODER_DEGREES);
Motor centre_l(12, E_MOTOR_GEARSET_06, true, E_MOTOR_ENCODER_DEGREES);
Motor back_l(13, E_MOTOR_GEARSET_06, false, E_MOTOR_ENCODER_DEGREES);

Motor front_r(18, E_MOTOR_GEARSET_06, true, E_MOTOR_ENCODER_DEGREES);
Motor centre_r(19, E_MOTOR_GEARSET_06, false, E_MOTOR_ENCODER_DEGREES);
Motor back_r(17, E_MOTOR_GEARSET_06, true, E_MOTOR_ENCODER_DEGREES);




Motor intake_m(1, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES);
Motor flywheel_m(2, E_MOTOR_GEARSET_06, false, E_MOTOR_ENCODER_DEGREES);

Piston indexer_p('B', "indexer_p", false, LOW);

ADIAnalogIn mag_ds('E');
ADIAnalogIn shooter_ds('C');

Rotation flywheel_rot_sensor(14);	// Configures rotation sensor in port 1

Piston angler_p('D', "angler_p", true, LOW);
Piston trans_p('A', "trans_p", true, HIGH);
Piston endgame_s_p('F', "endgame_s_p", false, LOW); // Single acting
Piston endgame_d_p('G', "endgame_d_p", false, LOW); // Double acting

Imu gyro(1);

Optical roller_sensor(9);

Distance l_reset_dist(7), r_reset_dist(8);
// Link radio(4, "Robot1", pros::E_LINK_RECIEVER);

Motor b_lift_m(16);
