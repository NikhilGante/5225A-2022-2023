#pragma once
#include "util.hpp"
#include "pros/apix.h"

using Port = std::uint8_t;

static constexpr Port expander_port = 2;

class _Controller;
class _Distance;
class Encoder;
class Gyro;
class _Motor;
class Piston;
class Light;
class Ultrasonic;

extern _Controller master;
extern _Controller partner;
extern _Motor front_l, front_r, back_l, back_r, centre_l, centre_r, flywheel_m, intake_m;
extern Encoder left_tracker, right_tracker, back_tracker;

extern Piston indexer_p;

extern Light mag_ds;
extern Light shooter_ds;
extern Light intk_ds;

extern Ultrasonic ultra_left;
extern Ultrasonic ultra_right;

extern Encoder flywheel_rot_sensor;

extern Piston angler_p, trans_p, endgame_s_p, endgame_d_p;

extern Gyro gyro;

extern _Distance l_reset_dist;
extern _Distance r_reset_dist;

Port valid_smart_port(std::string, Port);
Port valid_adi_port(std::string, Port);
ext_adi_port_pair_t valid_adi_ports(std::string, Port, Port);
Port valid_ext_adi_port(std::string, ext_adi_port_pair_t);
ext_adi_port_pair_t valid_ext_adi_ports(std::string, ext_adi_port_tuple_t);

inline bool correctDevice (Port port, c::v5_device_e device) {return c::registry_get_plugged_type(port-1) == device;}
bool correctDevice (auto* obj) {return correctDevice(obj->getPort(), obj->device);}