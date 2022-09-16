#pragma once
#include "Libraries/gui.hpp"
#include "main.h"
#include "Libraries/controller.hpp"

extern Controller master;
extern Controller partner;
extern pros::Motor front_l, front_r, back_l, back_r, centre_l, centre_r, flywheel_front, flywheel_back;
// extern pros::ADIEncoder LeftEncoder, RightEncoder, BackEncoder;
extern pros::Rotation left_tracker, right_tracker, back_tracker;

extern pros::Motor b_lift_m; //should this still be here?

inline void deprecated() {printf("Trying to use a deprecated feature.\n"); screen_flash::start("DEPRECATED");}
inline void not_implemented() {printf("Trying to use a feature that is not yet implemented.\n"); screen_flash::start("NOT IMPLEMENTED");}