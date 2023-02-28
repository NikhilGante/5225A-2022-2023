#pragma once
#include "main.h"

using Port = std::uint8_t;

static constexpr Port expander_port = 2;

class _Controller;
class _Distance;
class Encoder;
class Gyro;
class _Motor;
class Piston;
class Touch;
class Ultrasonic;

extern _Controller master;
extern _Controller partner;
extern _Motor front_l, front_r, back_l, back_r, centre_l, centre_r, flywheel_m, intake_m;
extern Encoder left_tracker, right_tracker, back_tracker;

extern Piston indexer_p;

extern Touch mag_ds;
extern Touch shooter_ds;

extern Ultrasonic ultra_left;
extern Ultrasonic ultra_right;

extern Encoder flywheel_rot_sensor;

extern Piston angler_p, trans_p, endgame_s_p, endgame_d_p;

extern Gyro gyro;

extern _Distance l_reset_dist;
extern _Distance r_reset_dist;

Port valid_smart_port(std::string, std::string, Port);
Port valid_adi_port(std::string, std::string, Port);
std::pair<Port, Port> valid_adi_ports(std::string, std::string, Port, Port);
Port valid_ext_adi_port(std::string, std::string, ext_adi_port_pair_t);
std::pair<Port, Port> valid_ext_adi_ports(std::string, std::string, ext_adi_port_tuple_t);
/*
 0   : Invalid Port
 1-21: Smart Ports
22-29: V5 ADI Ports
30-37: Expander ADI Ports
*/