#include "others.hpp"

Ultrasonic::Ultrasonic(Port port_ping, Port port_echo, std::string name):  ObjectTracker{class_name, name}, ADIUltrasonic{port_ping, port_echo} {std::tie(port1, port2) = valid_adi_ports    (getFullName(), port_ping, port_echo);}
Ultrasonic::Ultrasonic(ext_adi_port_tuple_t port_tuple, std::string name): ObjectTracker{class_name, name}, ADIUltrasonic{port_tuple          } {std::tie(port1, port2) = valid_ext_adi_ports(getFullName(), port_tuple          );}
Light::Light(Port port, std::string name):                                 ObjectTracker{class_name, name}, ADIAnalogIn  {port                } {this->port = valid_adi_port    (getFullName(), port     );}
Light::Light(ext_adi_port_pair_t port_pair, std::string name):             ObjectTracker{class_name, name}, ADIAnalogIn  {port_pair           } {this->port = valid_ext_adi_port(getFullName(), port_pair);}
Encoder::Encoder(Port port, std::string name):                             ObjectTracker{class_name, name}, Rotation     {port                } {this->port = valid_smart_port  (getFullName(), port     );}
Encoder::Encoder(Port port, std::string name, bool reverse):               ObjectTracker{class_name, name}, Rotation     {port                } {this->port = valid_smart_port  (getFullName(), port     );}
_Distance::_Distance(Port port, std::string name):                         ObjectTracker{class_name, name}, Distance     {port                } {this->port = valid_smart_port  (getFullName(), port     );}
Gyro::Gyro(Port port, std::string name):                                   ObjectTracker{class_name, name}, Imu          {port                } {this->port = valid_smart_port  (getFullName(), port     );}

ext_adi_port_pair_t Ultrasonic::getPorts() const {return {port1, port2};}
Port Light::getPort() const {return port;}
Port Encoder::getPort() const {return port;}
Port _Distance::getPort() const {return port;}
Port Gyro::getPort() const {return port;}

int Ultrasonic::getVal() const {return get_value();}
int Light::getVal() const {return get_value();}
int Encoder::getVal() {return get_position();}
int _Distance::getVal() {return get();}
double Gyro::getVal() const {return get_rotation();}

int Encoder::getVel() {return get_velocity();}
bool Gyro::isCalibrating() const {return is_calibrating();}

std::int32_t Encoder::reset() {return Rotation::reset();}
std::int32_t Encoder::resetPos() {return reset_position(); set_data_rate(5);}
void Gyro::tare() {tare_rotation();}