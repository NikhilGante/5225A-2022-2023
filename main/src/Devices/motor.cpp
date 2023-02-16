#include "motor.hpp"
#include "../Libraries/logging.hpp"
#include <sstream>

extern Page temps, motors;
extern Slider mot_speed_set;

_Motor::_Motor(std::int8_t port, std::string name, bool reversed, motor_gearset_e_t gearset, motor_encoder_units_e_t encoder_units):
Motor{port, gearset, reversed, encoder_units}, name{name}{
  {
    std::stringstream ss{getName()};
    using iterator = std::istream_iterator<std::string>;
    for(auto it = iterator{ss}; it != iterator{}; it++) short_name += it->front();
  }

  //2x4
  on         .construct(115*(getID()%4) + 15, getID() < 4 ? 120 : 205, 45, 30, GUI::Style::SIZE  , Button::SINGLE, &motors, "Run"                        , Color::dark_orange                                , Color::black);
  off        .construct(115*(getID()%4) + 70, getID() < 4 ? 120 : 205, 45, 30, GUI::Style::SIZE  , Button::SINGLE, &motors, "Stop"                       , Color::dark_orange                                , Color::black);
  text       .construct(115*(getID()%4) + 65, getID() < 4 ? 95  : 180,         GUI::Style::CENTRE, TEXT_SMALL    , &motors, getName()                    , nullptr                                           , Color::white);
  data       .construct(115*(getID()%4) + 65, getID() < 4 ? 110 : 195,         GUI::Style::CENTRE, TEXT_SMALL    , &motors, std::to_string(port) + ": %d", std::function([this](){return getRPM();})         , Color::white);
  temperature.construct(115*(getID()%4) + 65, getID() < 4 ? 125 : 190,         GUI::Style::CENTRE, TEXT_SMALL    , &temps , getShortName() + ": %dC"     , std::function([this](){return get_temperature();}), Color::black);

  on .setFunc([this](){move(mot_speed_set.getValue());});
  off.setFunc([this](){move(0                       );});
  temperature.setBackground(40, 20);

  if(!plugged()){
    on         .setActive(false);
    off        .setActive(false);
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
  device_log("Motor %s moving from %d to %d speed", getName(), speed, voltage);
  Motor::move(voltage);
  speed = voltage;
}

void _Motor::moveRelative(int velocity){
  int new_speed = velocityToVoltage(velocity);
  device_log("Motor %s relative moving from %d to %d speed", getName(), speed, new_speed);
  Motor::move_relative(velocity, 200);
  speed = new_speed;
}

void _Motor::brake(){
  device_log("Motor %s braking", getName());
  moveRelative(0);
}

int _Motor::getTemperature() const {
  int temp = get_temperature();
  return temp != std::numeric_limits<int>::max() ? temp : -1;
}

int _Motor::getRPM() const {
  int rpm = get_actual_velocity();
  return rpm != std::numeric_limits<int>::max() ? rpm : -1;
}

bool _Motor::plugged() const {return static_cast<int>(get_temperature()) != std::numeric_limits<int>::max();}
double _Motor::getPosition() const {return get_position();}
double _Motor::getTargetPosition() const {return get_target_position();}
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
    default: temperature.setBackground(static_cast<Color>((std::uint32_t)rand())); break;
  }

  if (getTemperature() > 55){
    temps.goTo();
    alert::start(5000, "%s motor is at %dC\n", getName(), getTemperature());
  }
}