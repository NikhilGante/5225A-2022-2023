#include "piston.hpp"
#include "../Libraries/logging.hpp"

extern Page pneumatics;

void Piston::construct(std::string name, bool reversed, ext_adi_port_pair_t port){
  this->name = name;
  this->reversed = reversed;

  //3x3
  toggle.construct(155*(getID()%3) + 10, 50*std::floor(getID()/3) + 30, 145, 40, GUI::Style::SIZE, Button::TOGGLE, &pneumatics, getName() + ": {" + std::to_string(port.first) + "," + static_cast<char>(port.second) + '}', Color::dark_orange, Color::black);

  toggle.setFunc([this](){
    device_log("Piston %s switching from %d to %d", getName(), getState(), HIGH != this->reversed);
    this->state = HIGH != this->reversed;
    set_value(getState());
  });
  toggle.setOffFunc([this](){
    device_log("Piston %s switching from %d to %d", getName(), getState(), LOW != this->reversed);
    this->state = LOW != this->reversed;
    set_value(getState());
  });
}

Piston::Piston(std::uint8_t port, std::string name, bool reversed, bool init_state):
ADIDigitalOut{port, init_state} {construct(name, reversed, {0, port});}

Piston::Piston(ext_adi_port_pair_t port_pair, std::string name, bool reversed, bool init_state):
ADIDigitalOut{port_pair, init_state} {construct(name, reversed, port_pair);}

bool Piston::getState() const {return this->state != reversed;}
void Piston::toggleState() {setState(!getState());}
std::string Piston::getName() const {return this->name;}

void Piston::setState(bool state){
  if(state) toggle.select();
  else toggle.deselect();
}