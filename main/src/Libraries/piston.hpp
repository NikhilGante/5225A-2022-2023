#pragma once
#include "main.h"

class Button;

//maybe a get_port if needed?
class Piston : public ADIDigitalOut{
  private:    
    bool state;  // the state of the SUBSYSTEM (e.g. 1 if the claw is closed, 0 if open)
    bool reversed;  // if the state of the subsytem is opposite the state of the piston
    int change_time = 0;
    std::string name;

    static int count;
  public:
    static std::array<std::pair<Piston*, Button*>, 8> list_for_gui;

    Piston(std::uint8_t adi_port, std::string name, bool reversed = false, bool init_state = LOW);
    Piston(ext_adi_port_pair_t port_pair, std::string name, bool reversed = false, bool init_state = LOW);

    void setState(bool state);
    bool getState() const;
    bool toggleState();

    int getStateTime() const;
    std::string getName() const;

    //static overloads (mostly for Nathan)
    static std::string get_name(int number);
};