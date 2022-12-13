#include "piston.hpp"
#include "logging.hpp"

int Piston::count = 0;
std::array<Piston*, 8> Piston::list_for_gui{};

extern Page pneumatics;

Piston::Piston(std::uint8_t port, std::string name, bool reversed, bool init_state):
ADIDigitalOut{port, init_state}, reversed{reversed}, name{name} {
  if(count < 8) list_for_gui[count] = this;
  
  toggle.construct(115*(count%4) + 15, count < 4 ? 45 : 140, 100, 75, GUI::Style::SIZE  , Button::SINGLE, &pneumatics, "" , Color::dark_orange, Color::black);
  text.construct(0, 0, GUI::Style::CENTRE, TEXT_SMALL, &pneumatics, getName(), nullptr, Color::white);

  toggle.setFunc([this](){setState(HIGH);});
  toggle.setOffFunc([this](){setState(LOW);});
  toggle.addText(text);

  count++;
}

Piston::Piston(ext_adi_port_pair_t port_pair, std::string name, bool reversed, bool init_state):
ADIDigitalOut{port_pair, init_state}, reversed{reversed}, name{name} {
  if(count < 8) list_for_gui[count] = this;
  
  toggle.construct(115*(count%4) + 15, count < 4 ? 45 : 140, 100, 75, GUI::Style::SIZE  , Button::SINGLE, &pneumatics, "" , Color::dark_orange, Color::black);
  text.construct(0, 0, GUI::Style::CENTRE, TEXT_SMALL, &pneumatics, getName(), nullptr, Color::white);

  toggle.setFunc([this](){setState(HIGH);});
  toggle.setOffFunc([this](){setState(LOW);});
  toggle.addText(text);

  count++;
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