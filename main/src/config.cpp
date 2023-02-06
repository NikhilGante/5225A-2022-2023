#include "Libraries/controller.hpp"
#include "Libraries/motor.hpp"
#include "Libraries/piston.hpp"

uint8_t rotation_port = 7;

_Controller master {CONTROLLER_MASTER};
_Controller partner{CONTROLLER_PARTNER};

ADIEncoder LeftEncoder{1, 2, false}, RightEncoder{5, 6, true}, BackEncoder{3, 4, false};

// tank-drive motor config
Rotation left_tracker{6}, right_tracker{1}, back_tracker{10};
_Motor front_l{20, "Front Left"};
_Motor centre_l{19, "Centre Left", true};
_Motor back_l{17, "Back Left"};

_Motor front_r{11, "Front Right", true};
_Motor centre_r{12, "Centre Right"};
_Motor back_r{13, "Back Right", true};

_Motor intake_m{5, "Intake", true, MOTOR_GEARSET_18};
_Motor flywheel_m{9, "Flywheel"};

Piston indexer_p{'D', "Indexer", false, LOW};

ADIAnalogIn mag_ds{'A'};
ADIAnalogIn shooter_ds{'B'};

Rotation flywheel_rot_sensor{rotation_port};

Piston angler_p{'E', "Angler", false, LOW};
Piston trans_p{'C', "Transmission", true, HIGH};
Piston endgame_s_p{'F', "Endgame Single", false, LOW}; // Single acting
Piston endgame_d_p{'G', "Endgame Double", false, LOW}; // Double acting

Optical roller_sensor{8};

Distance l_reset_dist{3}, r_reset_dist{7};