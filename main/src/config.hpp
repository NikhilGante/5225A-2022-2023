#pragma once
#include "main.h"
#include <map>

static constexpr uint8_t rotation_port = 7;

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

extern ADIUltrasonic ultra_left;
extern ADIUltrasonic ultra_right;

extern Rotation flywheel_rot_sensor;

extern Piston angler_p, trans_p, endgame_s_p, endgame_d_p;


extern Imu gyro;

extern Distance l_reset_dist;
extern Distance r_reset_dist;

std::uint8_t valid_smart_port(std::string, std::string, std::uint8_t);
std::uint8_t valid_adi_port(std::string, std::string, std::uint8_t);
std::uint8_t valid_ext_adi_port(std::string, std::string, ext_adi_port_pair_t);
extern std::map<std::uint8_t, std::string> port_list;
/*
 0   : Invalid Port
 1-21: Smart Ports
22-29: V5 ADI Ports
30-37: Expander ADI Ports
*/