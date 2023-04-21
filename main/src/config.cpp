#include "config.hpp"


_Controller master(E_CONTROLLER_MASTER);
_Controller partner(E_CONTROLLER_PARTNER);

static constexpr int e_port = 1;

Rotation left_tracker(10), right_tracker(19), back_tracker(4);

Motor front_l(16, E_MOTOR_GEARSET_06, false, E_MOTOR_ENCODER_DEGREES);
Motor centre_l(15, E_MOTOR_GEARSET_06, true, E_MOTOR_ENCODER_DEGREES);
Motor back_l(14, E_MOTOR_GEARSET_06, false, E_MOTOR_ENCODER_DEGREES);

Motor front_r(11, E_MOTOR_GEARSET_06, true, E_MOTOR_ENCODER_DEGREES);
Motor centre_r(12, E_MOTOR_GEARSET_06, false, E_MOTOR_ENCODER_DEGREES);
Motor back_r(13, E_MOTOR_GEARSET_06, true, E_MOTOR_ENCODER_DEGREES);


Motor intake_m(17, E_MOTOR_GEARSET_18, true, E_MOTOR_ENCODER_DEGREES);
Motor flywheel_m(18, E_MOTOR_GEARSET_06, false, E_MOTOR_ENCODER_DEGREES);

Piston indexer_p({e_port, 'G'}, "indexer_p", false, LOW);

ADIAnalogIn uptk_ds('C');
ADIAnalogIn intk_ds('D');
ADIDigitalIn intk_lim_switch('H');
Distance intk_dist(3);


ADIAnalogIn mag_ds({e_port, 'D'});
ADIAnalogIn shooter_ds('A');

Rotation flywheel_rot_sensor(7);

Piston angler_p({e_port, 'E'}, "angler_p", false, LOW); 
Piston trans_p({e_port, 'F'}, "trans_p", true, HIGH);


Piston endgame_top_b('E', "endgame_top_b", false, LOW);
Piston endgame_f('F', "endgame_f", false, LOW);
Piston endgame_lower_b('G', "endgame_lower_b", false, LOW);





Imu gyro(9), gyro2(2), gyro3(10);




// Index, Angler*2, Transmission, Magazine, Shooter, Intake