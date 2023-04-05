#include "config.hpp"


_Controller master(E_CONTROLLER_MASTER);
_Controller partner(E_CONTROLLER_PARTNER);

static constexpr int e_port = 1;

Rotation left_tracker(10), right_tracker(19), back_tracker(3);

Motor front_l(16, E_MOTOR_GEARSET_06, false, E_MOTOR_ENCODER_DEGREES);
Motor centre_l(15, E_MOTOR_GEARSET_06, true, E_MOTOR_ENCODER_DEGREES);
Motor back_l(14, E_MOTOR_GEARSET_06, false, E_MOTOR_ENCODER_DEGREES);

Motor front_r(11, E_MOTOR_GEARSET_06, true, E_MOTOR_ENCODER_DEGREES);
Motor centre_r(12, E_MOTOR_GEARSET_06, false, E_MOTOR_ENCODER_DEGREES);
Motor back_r(13, E_MOTOR_GEARSET_06, true, E_MOTOR_ENCODER_DEGREES);


Motor intake_m(17, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES);
Motor flywheel_m(18, E_MOTOR_GEARSET_06, false, E_MOTOR_ENCODER_DEGREES);

Piston indexer_p({e_port, 'G'}, "indexer_p", false, LOW);

ADIAnalogIn intk_ds('G');

ADIAnalogIn mag_ds({e_port, 'D'});
ADIAnalogIn shooter_ds('A');

Rotation flywheel_rot_sensor(7);

Piston angler_p({e_port, 'E'}, "angler_p", false, LOW); 
Piston trans_p({e_port, 'F'}, "trans_p", true, HIGH);
Piston endgame_s_p('B', "endgame_s_p", false, LOW); // Single acting
Piston endgame_d_p('C', "endgame_d_p", false, LOW); // Double acting

Imu gyro(9);

Distance l_reset_dist(1), r_reset_dist(20);



// Index, Angler*2, Transmission, Magazine, Shooter, Intake