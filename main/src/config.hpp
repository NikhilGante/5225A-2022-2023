#pragma once
#include "main.h"

class _Controller;
class _Motor;
class Piston;

extern _Controller master;
extern _Controller partner;
extern _Motor front_l, front_r, back_l, back_r, centre_l, centre_r, flywheel_m, intake_m;
// extern pros::ADIEncoder LeftEncoder, RightEncoder, BackEncoder;
extern Rotation left_tracker, right_tracker, back_tracker;

extern Piston indexer_p;

extern ADIAnalogIn mag_ds;
extern ADIAnalogIn shooter_ds;

extern Rotation flywheel_rot_sensor;

extern Piston angler_p, transmission;

extern Optical roller_sensor;


#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define DEBUG printf("Time:%07d - File:%s | Function:%s | Line:%d\n", millis(), __FILENAME__, __PRETTY_FUNCTION__, __LINE__);
#define DEPRECATE do{printf("Trying to use a deprecated feature:  "); DEBUG; alert::start("DEPRECATED");} while(0);
#define TO_IMPLEMENT do{printf("Trying to use a feature that is not yet implemented: "); DEBUG; alert::start("NOT IMPLEMENTED");} while(0);