#include "motor.hpp"
int _Motor::count = 0;

// extern Button pneum_1, pneum_2, pneum_3, pneum_4, pneum_5, pneum_6, pneum_7, pneum_8;

std::array<std::tuple<_Motor*, Button*, Text_*>, 8> _Motor::list_for_gui {{
  {nullptr, nullptr, nullptr},
  {nullptr, nullptr, nullptr},
  {nullptr, nullptr, nullptr},
  {nullptr, nullptr, nullptr},
  {nullptr, nullptr, nullptr},
  {nullptr, nullptr, nullptr},
  {nullptr, nullptr, nullptr},
  {nullptr, nullptr, nullptr},
}};

_Motor::_Motor(std::int8_t port, std::string name, bool reversed, motor_gearset_e_t gearset, motor_encoder_units_e_t encoder_units):
Motor{port, gearset, reversed, encoder_units}, name{name} {
  if(count < 8) std::get<_Motor*>(list_for_gui[count]) = this;
  count++;
}

void _Motor::move(int voltage) {Motor::move(voltage);}
void _Motor::move_relative(int velocity) {Motor::move_relative(0, velocity);}


int _Motor::getStateTime() const{
  return millis() - this->change_time;
}

std::string _Motor::getName() const{
  return this->name;
}

std::string _Motor::get_name(int number){
  return std::get<_Motor*>(list_for_gui[number-1]) ? std::get<_Motor*>(list_for_gui[number-1])->getName() : "No Motor";
}