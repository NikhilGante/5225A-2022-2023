#pragma once
#include "main.h"
#include "gui.hpp"

class Piston : public ADIDigitalOut{
  private:
    Button toggle;
    Text<> text;
    std::string name;
    static int count;

    bool state;  // the state of the SUBSYSTEM (e.g. 1 if the claw is closed, 0 if open)
    bool reversed;  // if the state of the subsytem is opposite the state of the piston

  public:
    static std::array<Piston*, 8> list_for_gui;

    Piston(std::uint8_t port, std::string name, bool reversed = false, bool init_state = LOW);
    Piston(ext_adi_port_pair_t port_pair, std::string name, bool reversed = false, bool init_state = LOW);

    void setState(bool state);
    void toggleState();
    bool getState() const;

    std::string getName() const;
};