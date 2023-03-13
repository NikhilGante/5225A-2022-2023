#include "piston.hpp"
#include "../config.hpp"
#include "../Libraries/logging.hpp"
#include "okapi/api/util/mathUtil.hpp"

extern Page pneumatics;

Piston::Piston(Port port, std::string name, bool reversed, bool init_state):
ObjectTracker{"Piston", name}, ADIDigitalOut{port, init_state}, reversed{reversed} {
  char port_char = valid_adi_port(getLongName(), port) + 'A' - 1;
  toggle.construct({static_cast<int>(155*((getID()-1)%3) + 10), static_cast<int>(50*std::floor((getID()-1)/3) + 30), 145, 40, GUI::Style::SIZE}, Button::TOGGLE, &pneumatics, getName() + ": " + port_char, Color::dark_orange, Color::black);

  toggle.setFunc([this](){
    state_log("%d: Piston %s switching from %d to %d", millis(), getName(), getState(), HIGH != this->reversed);
    this->state = HIGH != this->reversed;
    set_value(getState());
  });
  toggle.setOffFunc([this](){
    state_log("%d: Piston %s switching from %d to %d", millis(), getName(), getState(), LOW != this->reversed);
    this->state = LOW != this->reversed;
    set_value(getState());
  });
}

Piston::Piston(ext_adi_port_pair_t port_pair, std::string name, bool reversed, bool init_state):
ObjectTracker{"Piston", name}, ADIDigitalOut{port_pair, init_state}, reversed{reversed} {
  char port_char = valid_ext_adi_port(getLongName(), port_pair) + 'A' - 1;
  toggle.construct({static_cast<int>(155*((getID()-1)%3) + 10), static_cast<int>(50*std::floor((getID()-1)/3) + 30), 145, 40, GUI::Style::SIZE}, Button::TOGGLE, &pneumatics, getName() + ": {" + std::to_string(port_pair.first) + ", " + port_char + '}', Color::dark_orange, Color::black);

  toggle.setFunc([this](){
    state_log("%d: %s Piston switching from %d to %d", millis(), getName(), getState(), HIGH != this->reversed);
    this->state = HIGH != this->reversed;
    set_value(getState());
  });
  toggle.setOffFunc([this](){
    state_log("%d: %s Piston switching from %d to %d", millis(), getName(), getState(), LOW != this->reversed);
    this->state = LOW != this->reversed;
    set_value(getState());
  });
}

bool Piston::getState() const {return state != reversed;}
void Piston::toggleState() {setState(!getState());}

void Piston::setState(bool state){
  if(state) toggle.select();
  else toggle.deselect();
}