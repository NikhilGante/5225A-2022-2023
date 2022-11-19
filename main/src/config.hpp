#pragma once
#include "main.h"
#include "Libraries/controller.hpp"
#include "Libraries/piston.hpp"
using namespace pros;

extern _Controller master;
extern _Controller partner;
extern pros::Motor front_l, front_r, back_l, back_r, centre_l, centre_r, flywheel_m, intake_m;
// extern pros::ADIEncoder LeftEncoder, RightEncoder, BackEncoder;
extern pros::Rotation left_tracker, right_tracker, back_tracker;

extern Piston indexer_p;

extern ADIAnalogIn mag_ds;

extern pros::Rotation flywheel_rot_sensor;

extern pros::Motor b_lift_m;