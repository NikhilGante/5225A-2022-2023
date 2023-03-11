#include "motor.hpp"
#include "../Libraries/printing.hpp"
#include "../Libraries/logging.hpp"
#include "../util.hpp"
#include "../config.hpp"
#include <sstream>
#include <stdexcept>

extern Page temps, motors;
extern Slider mot_speed_set;

_Motor::_Motor(std::int8_t port, std::string name, bool reversed, motor_gearset_e_t gearset, motor_encoder_units_e_t encoder_units):
ObjectTracker{class_name, name}, Motor{port, gearset, reversed, encoder_units}, name{name}{
  valid_smart_port(class_name + ": " + name, port);

  {
    std::stringstream ss{getName()};
    using iterator = std::istream_iterator<std::string>;
    for(auto it = iterator{ss}; it != iterator{}; it++) short_name += it->front();
  }

  //2x4
  on         .construct({static_cast<int>(115*((getID()-1)%4) + 15), getID() <= 4 ? 120 : 205, 45, 30, GUI::Style::SIZE},  Button::SINGLE, &motors, "Run"                        , Color::dark_orange                                , Color::black);
  off        .construct({static_cast<int>(115*((getID()-1)%4) + 70), getID() <= 4 ? 120 : 205, 45, 30, GUI::Style::SIZE},  Button::SINGLE, &motors, "Stop"                       , Color::dark_orange                                , Color::black);
  text       .construct({static_cast<int>(115*((getID()-1)%4) + 65), getID() <= 4 ? 95  : 180},        GUI::Style::CENTRE, TEXT_SMALL    , &motors, getName()                    , nullptr                                           , Color::white);
  data       .construct({static_cast<int>(115*((getID()-1)%4) + 65), getID() <= 4 ? 110 : 195},        GUI::Style::CENTRE, TEXT_SMALL    , &motors, std::to_string(port) + ": %d", std::function([this](){return getRPM();})         , Color::white);
  temperature.construct({static_cast<int>(115*((getID()-1)%4) + 65), getID() <= 4 ? 125 : 190},        GUI::Style::CENTRE, TEXT_SMALL    , &temps , getShortName() + ": %dC"     , std::function([this](){return get_temperature();}), Color::black);

  on .setFunc([this](){move(mot_speed_set.getValue());});
  off.setFunc([this](){move(0                       );});
  temperature.setBackground(40, 20);

  if(!plugged()){
    on         .setActive(false);
    off        .setActive(false);
    text       .setActive(false);
    data       .setActive(false);
    temperature.setActive(false);
  }
}

int _Motor::velocityToVoltage(int velocity){
  double scaled_velocity = velocity*127;
  switch(get_gearing()){
    case MOTOR_GEAR_100: scaled_velocity /= 100; break;
    case MOTOR_GEAR_200: scaled_velocity /= 200; break;
    case MOTOR_GEAR_600: scaled_velocity /= 600; break;
    case MOTOR_GEARSET_INVALID: scaled_velocity = std::numeric_limits<double>::infinity(); break;
  }

  return scaled_velocity;
}

void _Motor::move(int voltage){
  if (speed != voltage) device_log("%d: Motor %s moving from %d to %d speed", millis(), getName(), speed, voltage);
  Motor::move(voltage);
  speed = voltage;
}

void _Motor::moveRelative(int velocity){
  int new_speed = velocityToVoltage(velocity);
  if (speed != new_speed) device_log("%d: Motor %s relative moving from %d to %d speed", millis(), getName(), speed, new_speed);
  Motor::move_relative(velocity, 200);
  speed = new_speed;
}

void _Motor::brake(){
  device_log("%d: Motor %s braking requested", millis(), getName());
  moveRelative(0);
  device_log("%d: Motor %s braking request processed", millis(), getName());
}

int _Motor::getTemperature() const {return plugged() ? get_temperature() : -1;}
int _Motor::getRPM() const {return plugged() ? get_actual_velocity() : -1;}
bool _Motor::plugged() const {return static_cast<int>(get_temperature()) != std::numeric_limits<int>::max();}
double _Motor::getPosition() const {return get_position();}
double _Motor::getTargetPosition() const {return get_target_position();}
Port _Motor::getPort() const {return get_port();}
std::string _Motor::getName() const {return name;}
std::string _Motor::getShortName() const {return short_name;}

void _Motor::updateTemperatureText(){
  switch(getTemperature()){
    case 0:
    case 5:  temperature.setBackground(Color::white); break;
    case 10:
    case 15: temperature.setBackground(Color::blue); break;
    case 20: temperature.setBackground(Color::dodger_blue); break;
    case 25: temperature.setBackground(Color::turquoise); break;
    case 30: temperature.setBackground(Color::medium_sea_green); break;
    case 35: temperature.setBackground(Color::lawn_green); break;
    case 40: temperature.setBackground(Color::lime_green); break;
    case 45: temperature.setBackground(Color::yellow); break;
    case 50: temperature.setBackground(Color::orange_red); break;
    case 55: temperature.setBackground(Color::red); break;
    default: temperature.setBackground(static_cast<Color>(rand())); break;
  }

  if (getTemperature() > 50){
    temps.goTo();
    alert::start(5000, "%s motor is at %dC\n", getName(), getTemperature());
  }
}
