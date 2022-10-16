#include "piston.hpp"
int Piston::count = 0;

// extern Button pneum_1, pneum_2, pneum_3, pneum_4, pneum_5, pneum_6, pneum_7, pneum_8;

// array<std::pair<Piston*, Button*>, 8> Piston::list_for_gui {{
//   {nullptr, &pneum_1},
//   {nullptr, &pneum_2},
//   {nullptr, &pneum_3},
//   {nullptr, &pneum_4},
//   {nullptr, &pneum_5},
//   {nullptr, &pneum_6},
//   {nullptr, &pneum_7},
//   {nullptr, &pneum_8},
// }};

Piston::Piston(std::uint8_t adi_port, std::string name, bool reversed, bool init_state): ADIDigitalOut(adi_port, init_state), reversed{reversed}, name(name){
  // if(count < 8) list_for_gui[count].first = this;
  // count++;
}
// constructor overload for port pair
Piston::Piston(ext_adi_port_pair_t port_pair, std::string name, bool reversed, bool init_state): ADIDigitalOut(port_pair, init_state), reversed{reversed}, name(name){
  // if(count < 8) list_for_gui[count].first = this;
  // count++;
}

void Piston::setState(bool state){
  this->state = state != reversed;
  this->set_value(this->state);
  this->change_time = millis();
}

bool Piston::getState() const{
  return this->state != reversed;
}

bool Piston::toggleState(){
  this->state = !this->state;
  this->set_value(state);
  this->change_time = millis();
  return state;
}

int Piston::getStateTime() const{
  return millis() - this->change_time;
}

std::string Piston::getName() const{
  return this->name;
}

// std::string Piston::get_name(int number){
//   return list_for_gui[number-1].first ? Piston::list_for_gui[number-1].first->get_name() : "No Piston";
// }