#include "config.hpp"
#include "util.hpp"
#include "Devices/controller.hpp"
#include "Devices/motor.hpp"
#include "Devices/piston.hpp"
#include "Devices/others.hpp"

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

Light shooter_ds{'A', "Shooter"};
Light mag_ds{'E', "Magazine"};
Light intk_ds{'G', "Intake"};

Encoder left_tracker{3, "Left Track"}, right_tracker{10, "Right Track"}, back_tracker{19, "Back Track"};
Encoder flywheel_rot_sensor{7, "Flywheel"};
_Distance l_reset_dist{1, "Left Reset"}, r_reset_dist{20, "Right Reset"};
Gyro gyro{9, "Gyro"};



std::array<std::string, 38> port_list;
/*
 0   : Invalid Port
 1-21: Smart Ports
22-29: V5 ADI Ports
30-37: Expander ADI Ports
*/

std::string port_to_string(Port port){
  if(inRangeIncl(port, 1, 21))  return sprintf2("Brain: %d", std::to_string(port));
  if(inRangeIncl(port, 22, 29)) return sprintf2("ADI: %c", port + 'A' - 22);
  if(inRangeIncl(port, 30, 37)) return sprintf2("Expander: %c", port + 'A' - 30);
  return "Invalid Port";
}

Port valid_smart_port(std::string device_label, Port port){
  std::string port_label = port_to_string(port);
  std::string& saved_device = port_list[port];

  if(!inRangeIncl(port, 1, 21)) alert::start("Cannot initialize %s in port \"%s\". Invalid port number",     device_label, port_label              );
  if(saved_device != "")        alert::start("Cannot initialize %s in port \"%s\". %s already exists there", device_label, port_label, saved_device);
  saved_device = device_label;
  return port;
}

Port valid_adi_port(std::string device_label, Port port){
  port = okapi::transformADIPort(port);
  std::string port_label = port_to_string(port+21);
  std::string& saved_device = port_list[port+21];

  if(!inRangeIncl(port, 1, 8)) alert::start("Cannot initialize %s in port \"%s\". Invalid port number",     device_label, port_label              );
  if(saved_device != "")       alert::start("Cannot initialize %s in port \"%s\". %s already exists there", device_label, port_label, saved_device);

  saved_device = device_label;
  return port;
}

ext_adi_port_pair_t valid_adi_ports(std::string device_label, Port port1, Port port2){
  port1 = okapi::transformADIPort(port1);
  std::string port_label1 = port_to_string(port1+21);
  std::string& saved_device1 = port_list[port1+21];

  port2 = okapi::transformADIPort(port2);
  std::string port_label2 = port_to_string(port2+21);
  std::string& saved_device2 = port_list[port2+21];

  if(!inRangeIncl(port1, 1, 8)) alert::start("Cannot initialize %s in port \"%s\". Invalid port number",     device_label, port_label1               );
  if(!inRangeIncl(port2, 1, 8)) alert::start("Cannot initialize %s in port \"%s\". Invalid port number",     device_label, port_label2               );
  if(saved_device1 != "")       alert::start("Cannot initialize %s in port \"%s\". %s already exists there", device_label, port_label1, saved_device1);
  if(saved_device2 != "")       alert::start("Cannot initialize %s in port \"%s\". %s already exists there", device_label, port_label2, saved_device2);
  if(port1 % 2 == 0)            alert::start("Cannot initialize %s in port \"%s\". Must be in ports 'A', 'C', 'E', or 'G'", device_label, port_label1);
  if(port1 + 1 != port2)        alert::start("Cannot initialize %s in ports \"%s\" and \"%s\". The second plug must come immediately after the first", device_label, port_label1, port_label2);

  saved_device1 = device_label;
  saved_device2 = device_label;
  return {port1, port2};
}

Port valid_ext_adi_port(std::string device_label, ext_adi_port_pair_t port_pair){
  Port port = okapi::transformADIPort(port_pair.second);
  std::string port_label = port_to_string(port+29);
  std::string& saved_device = port_list[port+29];
  
  Port expander = port_pair.first;
  std::string expander_port_label = port_to_string(expander);
  std::string& saved_smart_device = port_list[expander];

  if(!inRangeIncl(expander, 1, 21)) alert::start("Cannot initialize %s expander in port \"%s\". Invalid smart port number", device_label, expander_port_label);
  if(!inRangeIncl(port, 1, 8))      alert::start("Cannot initialize %s in port \"%s\". Invalid port number",                device_label, port_label              );
  if(saved_device != "")            alert::start("Cannot initialize %s in port \"%s\". %s already exists there",            device_label, port_label, saved_device);
  if(saved_smart_device != "" && saved_smart_device != "Expander") alert::start("Cannot initialize %s expander in port \"%s\". %s already exists there", device_label, expander_port_label, saved_smart_device);

  saved_device = device_label;
  saved_smart_device = "Expander";
  return port;
}

ext_adi_port_pair_t valid_ext_adi_ports(std::string device_label, ext_adi_port_tuple_t port_tuple){
  Port port1 = okapi::transformADIPort(std::get<1>(port_tuple));
  std::string port_label1 = port_to_string(port1+29);
  std::string& saved_device1 = port_list[port1+29];

  Port port2 = okapi::transformADIPort(std::get<2>(port_tuple));
  std::string port_label2 = port_to_string(port2+29);
  std::string& saved_device2 = port_list[port2+29];

  Port expander = std::get<0>(port_tuple);
  std::string expander_port_label = port_to_string(expander);
  std::string& saved_smart_device = port_list[expander];

  if(!inRangeIncl(expander, 1, 21)) alert::start("Cannot initialize %s expander in port \"%s\". Invalid smart port number", device_label, expander_port_label       );
  if(!inRangeIncl(port1, 1, 8))     alert::start("Cannot initialize %s in port \"%s\". Invalid port number",                device_label, port_label1               );
  if(!inRangeIncl(port2, 1, 8))     alert::start("Cannot initialize %s in port \"%s\". Invalid port number",                device_label, port_label2               );
  if(saved_device1 != "")           alert::start("Cannot initialize %s in port \"%s\". %s already exists there",            device_label, port_label1, saved_device1);
  if(saved_device2 != "")           alert::start("Cannot initialize %s in port \"%s\". %s already exists there",            device_label, port_label2, saved_device2);
  if(port1 % 2 == 0)                alert::start("Cannot initialize %s in port \"%s\". Must be in ports 'A', 'C', 'E', or 'G'", device_label, port_label1);
  if(port1 + 1 != port2)            alert::start("Cannot initialize %s in ports \"%s\" and \"%s\". The second plug must come immediately after the first", device_label, port_label1, port_label2);
  if(saved_smart_device != "" && saved_smart_device != "Expander") alert::start("Cannot initialize %s expander in port \"%s\". %s already exists there", device_label, expander_port_label, saved_smart_device);

  saved_device1 = device_label;
  saved_device2 = device_label;
  saved_smart_device = "Expander";
  return {port1, port2};
}