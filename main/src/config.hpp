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


#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define DEBUG printf("DEBUG INFO - File:%s | Function:%s | Line:%d\n", __FILENAME__, __PRETTY_FUNCTION__, __LINE__);
#define DEPRECATE do{printf("Trying to use a deprecated feature:  "); DEBUG; alert::start("DEPRECATED");} while(0);
#define TO_IMPLEMENT do{printf("Trying to use a feature that is not yet implemented: "); DEBUG; alert::start("NOT IMPLEMENTED");} while(0);