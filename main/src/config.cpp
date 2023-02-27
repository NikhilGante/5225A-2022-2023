#include "config.hpp"
#include "util.hpp"
#include "Devices/controller.hpp"
#include "Devices/motor.hpp"
#include "Devices/piston.hpp"


_Controller master {CONTROLLER_MASTER};
_Controller partner{CONTROLLER_PARTNER};

ADIEncoder LeftEncoder{1, 2, false}, RightEncoder{5, 6, true}, BackEncoder{3, 4, false};

// tank-drive motor config
Rotation left_tracker{6}, right_tracker{1}, back_tracker{10};
_Motor front_l{20, "Front Left"};
_Motor centre_l{19, "Centre Left", true};
_Motor back_l{17, "Back Left"};

_Motor front_r{11, "Front Right", true};
_Motor centre_r{12, "Centre Right"};
_Motor back_r{13, "Back Right", true};

_Motor intake_m{5, "Intake", true, MOTOR_GEARSET_18};
_Motor flywheel_m{9, "Flywheel"};

Piston indexer_p{'D', "Indexer", false, LOW};

ADIAnalogIn mag_ds{'A'};
ADIAnalogIn shooter_ds{'B'};

Rotation flywheel_rot_sensor{rotation_port};

Piston angler_p{'E', "Angler", false, LOW};
Piston trans_p{'C', "Transmission", true, HIGH};
Piston endgame_s_p{'F', "Endgame Single", false, LOW}; // Single acting
Piston endgame_d_p{'G', "Endgame Double", false, LOW}; // Double acting

Optical roller_sensor{8};
Distance l_reset_dist{3}, r_reset_dist{7};
Imu gyro{14};
// Link radio{4, "Radio", pros::E_LINK_RECIEVER};

std::map<std::uint8_t, std::string> port_list;

std::uint8_t valid_smart_port(std::string class_name, std::string name, std::uint8_t port){
  if(inRangeIncl(port, 1, 21)){
    if(!port_list.contains(port)) return port;
    else throw std::invalid_argument(sprintf2("Cannot initialize %s %s in port %c. %s already exists there", class_name, name, port + 'A' - 1, port_list[port]));
  }
  else throw std::domain_error(sprintf2("Cannot initialize %s %s in port %c. Invalid port number", class_name, name, port + 'A' - 1));
}

std::uint8_t valid_adi_port(std::string class_name, std::string name, std::uint8_t port){
  std::uint8_t numerical_port = okapi::transformADIPort(port);
  std::uint8_t map_port = numerical_port + 21;
  char char_port = numerical_port + 'A' - 1;

  if(inRangeIncl(numerical_port, 1, 8)){
    if(!port_list.contains(map_port)) return map_port;
    else throw std::invalid_argument(sprintf2("Cannot initialize %s %s in ADI port %c. %s already exists there", class_name, name, char_port, port_list[map_port]));
  }
  else throw std::domain_error(sprintf2("Cannot initialize %s %s in ADI port %c. Invalid port number", class_name, name, char_port));
}

std::uint8_t valid_ext_adi_port(std::string class_name, std::string name, ext_adi_port_pair_t port_pair){
  std::uint8_t numerical_port = okapi::transformADIPort(port_pair.second);
  std::uint8_t map_port = numerical_port + 29;
  char char_port = numerical_port + 'A' - 1;

  if(inRangeIncl(port_pair.first, 1, 21) && inRangeIncl(numerical_port, 1, 8)){
    if(!port_list.contains(map_port)) return map_port;
    else throw std::invalid_argument(sprintf2("Cannot initialize %s %s in expander port %c. %s already exists there", class_name, name, char_port, port_list[map_port]));
  }
  else throw std::domain_error(sprintf2("Cannot initialize %s %s in expander port %c. Invalid port number", class_name, name, char_port));
}