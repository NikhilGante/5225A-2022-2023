#pragma once
#include "main.h"

static constexpr uint8_t rotation_port = 2;

class _Controller;
class _Motor;
class Piston;

extern _Controller master;
extern _Controller partner;
extern _Motor front_l, front_r, back_l, back_r, centre_l, centre_r, flywheel_m, intake_m;
extern Rotation left_tracker, right_tracker, back_tracker;

extern Piston indexer_p;

extern ADIAnalogIn mag_ds;
extern ADIAnalogIn shooter_ds;

extern Rotation flywheel_rot_sensor;

extern Piston angler_p, trans_p, endgame_s_p, endgame_d_p;

extern Optical roller_sensor;

extern Imu gyro;

extern Distance l_reset_dist;
extern Distance r_reset_dist;

#ifndef __FILENAME__
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif
#define DEBUG printf("Time:%07d - File:%s | Function:%s | Line:%d\n", millis(), __FILENAME__, __PRETTY_FUNCTION__, __LINE__);