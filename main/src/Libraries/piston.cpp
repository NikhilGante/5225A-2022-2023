#include "piston.hpp"
#include "logging.hpp"

extern Page pneumatics;

void Piston::construct(std::string name, bool reversed){
  this->name = name;
  this->reversed = reversed;

  toggle.construct(115*(getCount()%4) + 15, getCount() < 4 ? 45 : 140, 100, 75, GUI::Style::SIZE  , Button::SINGLE, &pneumatics, "" , Color::dark_orange, Color::black);
  text.construct(0, 0, GUI::Style::CENTRE, TEXT_SMALL, &pneumatics, getName(), nullptr, Color::white);

  toggle.setFunc([this](){setState(HIGH);});
  toggle.setOffFunc([this](){setState(LOW);});
  toggle.addText(text);
}

Piston::Piston(std::uint8_t port, std::string name, bool reversed, bool init_state):
ADIDigitalOut{port, init_state}, Counter{name} {
  construct(name, reversed);
}


Piston::Piston(ext_adi_port_pair_t port_pair, std::string name, bool reversed, bool init_state):
ADIDigitalOut{port_pair, init_state}, Counter{name} {
  construct(name, reversed);
}

void Piston::setState(bool state){
  sensor_data.print("Piston %s switching from %d to %d", getName(), getState(), state != reversed);
  this->state = state != reversed;
  set_value(getState());
}

bool Piston::getState() const{
  return this->state != reversed;
}

void Piston::toggleState(){
  setState(!getState());
}

std::string Piston::getName() const{
  return this->name;
}