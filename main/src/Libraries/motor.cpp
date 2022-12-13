#include "motor.hpp"
#include "logging.hpp"
#include <sstream>

int _Motor::count = 0;
std::array<_Motor*, 8> _Motor::list_for_gui{};

extern Page temps, motors;
extern Slider mot_speed_set;

_Motor::_Motor(std::int8_t port, std::string name, bool reversed, motor_gearset_e_t gearset, motor_encoder_units_e_t encoder_units):
Motor{port, gearset, reversed, encoder_units}, name{name} {
  if(count < 8) list_for_gui[count] = this;
  
  on         .construct(115*(count%4) + 15, count < 4 ? 125 : 190, 45, 30, GUI::Style::SIZE  , Button::SINGLE, &motors, "Run" , Color::dark_orange, Color::black);
  off        .construct(115*(count%4) + 70, count < 4 ? 125 : 190, 45, 30, GUI::Style::SIZE  , Button::SINGLE, &motors, "Stop", Color::dark_orange, Color::black);
  text       .construct(115*(count%4) + 70, count < 4 ? 125 : 190,         GUI::Style::CENTRE, TEXT_SMALL    , &motors, getName(), nullptr, Color::white);
  temperature.construct(115*(count%4) + 70, count < 4 ? 125 : 190,         GUI::Style::CENTRE, TEXT_SMALL    , &temps , getShortName() + ": %dC", std::function([this](){return get_temperature();}), Color::white);

  on.setFunc([this](){move(mot_speed_set.getValue());});
  off.setFunc([this](){move(0);});
  temperature.setBackground(40, 20);

  
  std::stringstream ss(getName());
  for(auto begin = std::istream_iterator<std::string>{ss}; begin != std::istream_iterator<std::string>{}; begin++) short_name += begin->front();

  count++;
}

void _Motor::move(int voltage){
  sensor_data.print("Motor %s moving from %d to %d speed", getName(), speed, voltage);
  Motor::move(voltage);
  speed = voltage;
}

void _Motor::brake(){
  sensor_data.print("Motor %s braking", getName());
  Motor::move_relative(0, 200);
}

int _Motor::getTemperature() const {return get_temperature();}
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
