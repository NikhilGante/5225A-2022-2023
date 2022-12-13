#include "config.hpp"
#include "Libraries/controller.hpp"
#include "Libraries/motor.hpp"
#include "Libraries/piston.hpp"

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

_Motor intakm{5, "Intake", true, MOTOR_GEARSET_18};
_Motor flywheel_m{9, "Flywheel"};

Piston indexer_p{'E', "Indexer"};

ADIAnalogIn mag_ds{'A'};

Rotation flywheel_rot_sensor{3};	// Configures rotation sensor in port 1

Piston angler_p{'H', "Angler", true};

// Motor b_lift_m{16};
