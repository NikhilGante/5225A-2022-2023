#include "piston.hpp"
#include "../config.hpp"
#include "../Libraries/logging.hpp"

extern Page pneumatics;

Piston::Piston(std::uint8_t port, std::string name, bool reversed, bool init_state):
ObjectTracker{"Piston", name}, ADIDigitalOut{port, init_state} {
  port = valid_adi_port("Piston", name, port);
  port_list[port] = name;

  toggle.construct({static_cast<int>(155*((getID()-1)%3) + 10), static_cast<int>(50*std::floor((getID()-1)/3) + 30), 145, 40, GUI::Style::SIZE}, Button::TOGGLE, &pneumatics, getName() + ": " + static_cast<char>(port + 43), Color::dark_orange, Color::black);
  
  this->name = name;
  this->reversed = reversed;

  toggle.setFunc([this](){
    device_log("%d: Piston %s switching from %d to %d", millis(), getName(), getState(), HIGH != this->reversed);
    this->state = HIGH != this->reversed;
    set_value(getState());
  });
  toggle.setOffFunc([this](){
    device_log("%d: Piston %s switching from %d to %d", millis(), getName(), getState(), LOW != this->reversed);
    this->state = LOW != this->reversed;
    set_value(getState());
  });
}

Piston::Piston(ext_adi_port_pair_t port_pair, std::string name, bool reversed, bool init_state):
ObjectTracker{"Piston", name}, ADIDigitalOut{port_pair, init_state} {
  std::uint8_t port = valid_ext_adi_port("Piston", name, port_pair);
  port_list[port] = name;

  toggle.construct({static_cast<int>(155*((getID()-1)%3) + 10), static_cast<int>(50*std::floor((getID()-1)/3) + 30), 145, 40, GUI::Style::SIZE}, Button::TOGGLE, &pneumatics, getName() + ": {" + std::to_string(port_pair.first) + ", " + static_cast<char>(port + 35) + '}', Color::dark_orange, Color::black);
  
  this->name = name;
  this->reversed = reversed;

  toggle.setFunc([this](){
    device_log("%d: Piston %s switching from %d to %d", millis(), getName(), getState(), HIGH != this->reversed);
    this->state = HIGH != this->reversed;
    set_value(getState());
  });
  toggle.setOffFunc([this](){
    device_log("%d: Piston %s switching from %d to %d", millis(), getName(), getState(), LOW != this->reversed);
    this->state = LOW != this->reversed;
    set_value(getState());
  });
}

bool Piston::getState() const {return state != reversed;}
void Piston::toggleState() {setState(!getState());}
std::string Piston::getName() const {return name;}

void Piston::setState(bool state){
  if(state) toggle.select();
  else toggle.deselect();
}