#pragma once
#include "main.h"
#include "Libraries/piston.hpp"
#include "Libraries/controller.hpp"
#include "pros/adi.hpp"


extern _Controller master;
extern _Controller partner;
extern pros::Motor front_l, front_r, back_l, back_r, flywheel_m;
extern pros::ADIEncoder LeftEncoder, RightEncoder, BackEncoder;
extern pros::ADIAnalogIn disc_sensor;

extern Piston indexer_p;