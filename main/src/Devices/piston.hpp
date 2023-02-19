#pragma once
#include "main.h"
#include "../Libraries/gui.hpp"
#include "../Libraries/tracker.hpp"

class Piston: private ADIDigitalOut, public ObjectTracker<Piston, 12>{
  private:
    Button toggle;
    std::string name;

    bool state;  // the state of the SUBSYSTEM (e.g. 1 if the claw is closed, 0 if open)
    bool reversed;  // if the state of the subsytem is opposite the state of the piston

    void construct(std::string name, bool reversed, ext_adi_port_pair_t port_pair);

  public:

    Piston(std::uint8_t port, std::string name, bool reversed = false, bool init_state = LOW);
    Piston(ext_adi_port_pair_t port_pair, std::string name, bool reversed = false, bool init_state = LOW);

    void setState(bool state);
    void toggleState();
    bool getState() const;

    std::string getName() const;
};