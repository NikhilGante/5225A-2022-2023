#pragma once
#include "main.h"
#include "Libraries/controller.hpp"
using namespace pros;

extern _Controller master;
extern _Controller partner;
extern pros::Motor front_l, front_r, back_l, back_r, flywheel_front, flywheel_back;
extern pros::ADIEncoder LeftEncoder, RightEncoder, BackEncoder;

extern pros::Motor b_lift_m;