#pragma once
#include "main.h"
#include "../Libraries/gui.hpp"
#include "../config.hpp"

class Piston: private ADIDigitalOut, public ObjectTracker<Piston>{
  private:
    Button toggle;
    std::string name;

    bool state;  //The state of the SUBSYSTEM (e.g. 1 if the claw is closed, 0 if open)
    bool reversed;  //If the state of the subsytem is opposite the state of the piston

  public:

    Piston(Port port, std::string name, bool reversed = false, bool init_state = LOW);
    Piston(ext_adi_port_pair_t port_pair, std::string name, bool reversed = false, bool init_state = LOW);

    void setState(bool state);
    void toggleState();
    bool getState() const;

    std::string getName() const;
};