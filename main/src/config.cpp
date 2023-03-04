#include "config.hpp"


_Controller master(E_CONTROLLER_MASTER);
_Controller partner(E_CONTROLLER_PARTNER);

static constexpr int e_port = 2;

// ADIEncoder LeftEncoder(1, 2, false), RightEncoder(5, 6, true), BackEncoder(3, 4, false);

// tank-drive motor config
Rotation left_tracker(3), right_tracker(10), back_tracker(19);




Motor front_l(16, E_MOTOR_GEARSET_06, false, E_MOTOR_ENCODER_DEGREES);
Motor centre_l(15, E_MOTOR_GEARSET_06, true, E_MOTOR_ENCODER_DEGREES);
Motor back_l(14, E_MOTOR_GEARSET_06, false, E_MOTOR_ENCODER_DEGREES);

Motor front_r(11, E_MOTOR_GEARSET_06, true, E_MOTOR_ENCODER_DEGREES);
Motor centre_r(12, E_MOTOR_GEARSET_06, false, E_MOTOR_ENCODER_DEGREES);
Motor back_r(13, E_MOTOR_GEARSET_06, true, E_MOTOR_ENCODER_DEGREES);




Motor intake_m(17, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES);
Motor flywheel_m(18, E_MOTOR_GEARSET_06, false, E_MOTOR_ENCODER_DEGREES);

Piston indexer_p({e_port, 'C'}, "indexer_p", true, LOW);

ADIAnalogIn intk_ds('G');

ADIAnalogIn mag_ds({e_port, 'D'});
ADIAnalogIn shooter_ds('A');

Rotation flywheel_rot_sensor(7);

ADIUltrasonic ultra_left({{e_port, 'E', 'F'}});
ADIUltrasonic ultra_right({{e_port,'G', 'H'}});

ADIUltrasonic match_ultra({'C', 'D'});

Piston angler_p({e_port, 'B'}, "angler_p", true, LOW);
Piston trans_p({e_port, 'A'}, "trans_p", true, HIGH);
Piston endgame_s_p('F', "endgame_s_p", false, LOW); // Single acting
Piston endgame_d_p('B', "endgame_d_p", false, LOW); // Double acting

Imu gyro(9);

// Optical roller_sensor(9);

Distance l_reset_dist(1), r_reset_dist(20);
// Link radio(4, "Robot1", pros::E_LINK_RECIEVER);

Motor b_lift_m(5);
