#pragma once
#include "main.h"
#include "Libraries/controller.hpp"
#include "Libraries/piston.hpp"
using namespace pros;

extern _Controller master;
extern _Controller partner;
extern Motor front_l, front_r, back_l, back_r, centre_l, centre_r, flywheel_m, intake_m;
// extern pros::ADIEncoder LeftEncoder, RightEncoder, BackEncoder;
extern Rotation left_tracker, right_tracker, back_tracker;

extern Piston indexer_p;

extern ADIAnalogIn intk_ds;
extern ADIAnalogIn mag_ds;
extern ADIAnalogIn shooter_ds;

extern ADIUltrasonic match_ultra;
extern ADIUltrasonic ultra_left;
extern ADIUltrasonic ultra_right;

extern Rotation flywheel_rot_sensor;

extern Piston angler_p, trans_p, endgame_s_p, endgame_d_p;


extern Imu gyro;

extern Distance l_reset_dist;
extern Distance r_reset_dist;

// extern Link radio;

extern Motor b_lift_m;