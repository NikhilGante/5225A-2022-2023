#pragma once
#include "main.h"
#include "Libraries/piston.hpp"
#include "Libraries/controller.hpp"


extern _Controller master;
extern _Controller partner;
extern pros::Motor front_l, front_r, back_l, back_r, flywheel_front, flywheel_back;
extern pros::ADIEncoder LeftEncoder, RightEncoder, BackEncoder;
