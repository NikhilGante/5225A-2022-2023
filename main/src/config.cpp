#include "config.hpp"
#include "util.hpp"
#include "Devices/controller.hpp"
#include "Devices/motor.hpp"
#include "Devices/piston.hpp"
#include "Devices/others.hpp"

#include <map>

_Controller master {CONTROLLER_MASTER};
_Controller partner{CONTROLLER_PARTNER};

// Tank Drive Motor Configuration
_Motor front_l{16, "Front Left"};
_Motor centre_l{15, "Centre Left", true};
_Motor back_l{14, "Back Left"};

_Motor front_r{11, "Front Right", true};
_Motor centre_r{12, "Centre Right"};
_Motor back_r{13, "Back Right", true};

_Motor intake_m{17, "Intake", true, MOTOR_GEARSET_18};
_Motor flywheel_m{18, "Flywheel"};

Piston trans_p{{expander_port, 'A'}, "Transmission", true, HIGH};
Piston angler_p{{expander_port, 'B'}, "Angler"};
Piston indexer_p{{expander_port, 'C'}, "Indexer", true};
Piston endgame_s_p{'B', "Endgame Single"}; // Single acting
Piston endgame_d_p{'C', "Endgame Double"}; // Double acting

Ultrasonic ultra_left{{expander_port, 'E', 'F'}, "Left"};
Ultrasonic ultra_right{{expander_port,'G', 'H'}, "Right"};

Touch shooter_ds{'A', "Shooter"};
Touch mag_ds{'E', "Magazine"};
Touch intk_ds{'G', "Intake"};

Encoder left_tracker{3, "Left Track"}, right_tracker{10, "Right Track"}, back_tracker{19, "Back Track"};
Encoder flywheel_rot_sensor{7, "Flywheel"};
_Distance l_reset_dist{1, "Left Reset"}, r_reset_dist{20, "Right Reset"};
Gyro gyro{9, "Gyro"};


// std::map<port, std::pair<class_name, name>>
std::map<Port, std::pair<std::string, std::string>> port_list;
/*
 0   : Invalid Port
 1-21: Smart Ports
22-29: V5 ADI Ports
30-37: Expander ADI Ports
*/

Port valid_smart_port(std::string class_name, std::string name, Port port){
  if(!inRangeIncl(port, 1, 21)) throw std::domain_error(sprintf2("Cannot initialize %s %s in port %c. Invalid port number", class_name, name, port + 'A' - 1));
  if(port_list.contains(port)) throw std::invalid_argument(sprintf2("Cannot initialize %s %s in port %c. %s %s already exists there", class_name, name, port + 'A' - 1, port_list[port].first, port_list[port].second));
  port_list[port] = {class_name, name};
  return port;
}

Port valid_adi_port(std::string class_name, std::string name, Port port){
  Port numerical_port = okapi::transformADIPort(port);
  Port port_index = numerical_port + 21;
  char char_port = numerical_port + 'A' - 1;

  if(!inRangeIncl(numerical_port, 1, 8)) throw std::domain_error(sprintf2("Cannot initialize %s %s in ADI port %c. Invalid port number", class_name, name, char_port));
  if(!port_list.contains(port_index)) throw std::invalid_argument(sprintf2("Cannot initialize %s %s in ADI port %c. %s %s already exists there", class_name, name, char_port, port_list[port_index].first, port_list[port_index].second));
  port_list[port_index] = {class_name, name};
  return port_index;
}

std::pair<Port, Port> valid_adi_ports(std::string class_name, std::string name, Port port1, Port port2){
  Port numerical_port_1 = okapi::transformADIPort(port1);
  Port port_index_1 = numerical_port_1 + 29;
  char char_port_1 = numerical_port_1 + 'A' - 1;

  Port numerical_port_2 = okapi::transformADIPort(port2);
  Port port_index_2 = numerical_port_2 + 29;
  char char_port_2 = numerical_port_2 + 'A' - 1;

  if(!inRangeIncl(numerical_port_1, 1, 8)) throw std::domain_error(sprintf2("Cannot initialize %s %s in ADI port %c. Invalid port number", class_name, name, char_port_1));
  if(!inRangeIncl(numerical_port_2, 1, 8)) throw std::domain_error(sprintf2("Cannot initialize %s %s in ADI port %c. Invalid port number", class_name, name, char_port_2));
  if(numerical_port_1 % 2 == 0) throw std::domain_error(sprintf2("Cannot initialize %s %s in ADI port %c. Must be in ports 'A', 'C', 'E', or 'G'", class_name, name, char_port_1));
  if(numerical_port_2 - numerical_port_1 != 1) throw std::domain_error(sprintf2("Cannot initialize %s %s in ADI ports %c and %c. The second plug must come after the first", class_name, name, char_port_1, char_port_2));
  if(port_list.contains(port_index_1)) throw std::invalid_argument(sprintf2("Cannot initialize %s %s in ADI port %c. %s %s already exists there", class_name, name, char_port_1, port_list[port_index_1].first, port_list[port_index_1].second));
  if(port_list.contains(port_index_2)) throw std::invalid_argument(sprintf2("Cannot initialize %s %s in ADI port %c. %s %s already exists there", class_name, name, char_port_2, port_list[port_index_2].first, port_list[port_index_2].second));
  port_list[port_index_1] = {class_name, name};
  port_list[port_index_2] = {class_name, name};
  return {port_index_1, port_index_2};
}

Port valid_ext_adi_port(std::string class_name, std::string name, ext_adi_port_pair_t port_pair){
  Port numerical_port = okapi::transformADIPort(port_pair.second);
  Port port_index = numerical_port + 29;
  char char_port = numerical_port + 'A' - 1;

  if(!inRangeIncl(port_pair.first, 1, 21)) throw std::domain_error(sprintf2("Cannot initialize %s %s expander in port %c. Invalid smart port number", class_name, name, port_pair.first));
  if(!inRangeIncl(numerical_port, 1, 8)) throw std::domain_error(sprintf2("Cannot initialize %s %s in expander port %c. Invalid port number", class_name, name, char_port));
  if(port_list.contains(port_index)) throw std::invalid_argument(sprintf2("Cannot initialize %s %s in expander port %c. %s %s already exists there", class_name, name, char_port, port_list[port_index].first, port_list[port_index].second));
  port_list[port_index] = {class_name, name};
  return port_index;
}

std::pair<Port, Port> valid_ext_adi_ports(std::string class_name, std::string name, ext_adi_port_tuple_t port_tuple){
  Port numerical_port_1 = okapi::transformADIPort(std::get<1>(port_tuple));
  Port port_index_1 = numerical_port_1 + 29;
  char char_port_1 = numerical_port_1 + 'A' - 1;

  Port numerical_port_2 = okapi::transformADIPort(std::get<2>(port_tuple));
  Port port_index_2 = numerical_port_2 + 29;
  char char_port_2 = numerical_port_2 + 'A' - 1;

  if(!inRangeIncl(std::get<0>(port_tuple), 1, 21)) throw std::domain_error(sprintf2("Cannot initialize %s %s expander in port %c. Invalid smart port number", class_name, name, std::get<0>(port_tuple)));
  if(!inRangeIncl(numerical_port_1, 1, 8)) throw std::domain_error(sprintf2("Cannot initialize %s %s in expander port %c. Invalid port number", class_name, name, char_port_1));
  if(!inRangeIncl(numerical_port_2, 1, 8)) throw std::domain_error(sprintf2("Cannot initialize %s %s in expander port %c. Invalid port number", class_name, name, char_port_2));
  if(numerical_port_1 % 2 == 0) throw std::domain_error(sprintf2("Cannot initialize %s %s in expander port %c. Must be in ports 'A', 'C', 'E', or 'G'", class_name, name, char_port_1));
  if(numerical_port_2 - numerical_port_1 != 1) throw std::domain_error(sprintf2("Cannot initialize %s %s in expander ports %c and %c. The second plug must come after the first", class_name, name, char_port_1, char_port_2));
  if(port_list.contains(port_index_1)) throw std::invalid_argument(sprintf2("Cannot initialize %s %s in expander port %c. %s %s already exists there", class_name, name, char_port_1, port_list[port_index_1].first, port_list[port_index_1].second));
  if(port_list.contains(port_index_2)) throw std::invalid_argument(sprintf2("Cannot initialize %s %s in expander port %c. %s %s already exists there", class_name, name, char_port_2, port_list[port_index_2].first, port_list[port_index_2].second));
  port_list[port_index_1] = {class_name, name};
  port_list[port_index_2] = {class_name, name};
  return {port_index_1, port_index_2};
}