#include "others.hpp"

Ultrasonic::Ultrasonic(Port port_ping, Port port_echo, std::string name):  ObjectTracker{name + ' ' + class_name}, ADIUltrasonic{port_ping, port_echo}, name{name} {std::tie(port1, port2) = valid_adi_ports    (name + class_name, port_ping, port_echo);}
Ultrasonic::Ultrasonic(ext_adi_port_tuple_t port_tuple, std::string name): ObjectTracker{name + ' ' + class_name}, ADIUltrasonic{port_tuple          }, name{name} {std::tie(port1, port2) = valid_ext_adi_ports(name + class_name, port_tuple          );}
Light::Light(Port port, std::string name):                                 ObjectTracker{name + ' ' + class_name}, ADIAnalogIn  {port                }, name{name} {                port   = valid_adi_port     (name + class_name, port                );}
Light::Light(ext_adi_port_pair_t port_pair, std::string name):             ObjectTracker{name + ' ' + class_name}, ADIAnalogIn  {port_pair           }, name{name} {                port   = valid_ext_adi_port (name + class_name, port_pair           );}
Encoder::Encoder(Port port, std::string name):                             ObjectTracker{name + ' ' + class_name}, Rotation     {port                }, name{name} {                port   = valid_smart_port   (name + class_name, port                );}
Encoder::Encoder(Port port, std::string name, bool reverse):               ObjectTracker{name + ' ' + class_name}, Rotation     {port                }, name{name} {                port   = valid_smart_port   (name + class_name, port                );}
_Distance::_Distance(Port port, std::string name):                         ObjectTracker{name + ' ' + class_name}, Distance     {port                }, name{name} {                port   = valid_smart_port   (name + class_name, port                );}
Gyro::Gyro(Port port, std::string name):                                   ObjectTracker{name + ' ' + class_name}, Imu          {port                }, name{name} {                port   = valid_smart_port   (name + class_name, port                );}

std::string Ultrasonic::getName() const {return name;}
std::string Light     ::getName() const {return name;}
std::string Encoder   ::getName() const {return name;}
std::string _Distance ::getName() const {return name;}
std::string Gyro      ::getName() const {return name;}
ext_adi_port_pair_t Ultrasonic::getPorts() const {return {port1, port2};}
Port Light::getPort() const {return port;}
Port Encoder::getPort() const {return port;}
Port _Distance::getPort() const {return port;}
Port Gyro::getPort() const {return port;}


int Ultrasonic::getVal() const {return get_value();}
int Light::getVal() const {return get_value();}
int Encoder::getVal() {return get_position();}
int Encoder::getVel() {return get_velocity();}
int _Distance::getVal() {return get();}
bool Gyro::isCalibrating() const {return is_calibrating();}
double Gyro::getVal() const {return get_rotation();}

std::int32_t Encoder::reset() {return Rotation::reset();}
std::int32_t Encoder::resetPos() {return reset_position(); set_data_rate(5);}
void Gyro::tare() {tare_rotation();}