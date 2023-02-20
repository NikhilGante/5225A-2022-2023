#include "menu.hpp"
#include "config.hpp"
#include "pros/misc.h"
#include "util.hpp"
#include "Devices/controller.hpp"
#include "Libraries/logging.hpp"

// std::vector<Auton*> Auton::logs{};

Auton::E_Reset_Types Auton::getResetType() const {return reset_type;} // Getter

void Auton::select(){
	int cur_auton = 0;
	master.clear();
	master.print(0, getNth(cur_auton) ? getNth(cur_auton)->name : "Null Auton*");
	master.print(1, "Up/dn change auton");
	master.print(2, "Press A to save");
	auton_log("Constructed %d Autons", Auton::getList().size()); //? Why is this line needed

  while(true){
    switch(master.waitForPress({DIGITAL_UP, DIGITAL_RIGHT, DIGITAL_DOWN, DIGITAL_LEFT, DIGITAL_A, DIGITAL_B, DIGITAL_X})){
      case DIGITAL_UP:
      case DIGITAL_RIGHT:
        if(cur_auton < Auton::getList().size() - 1) master.print(0, getNth(++cur_auton)->name);
        break;

      case DIGITAL_DOWN:
      case DIGITAL_LEFT:
        if(cur_auton > 0) master.print(0, getNth(--cur_auton)->name);
        break;

      case DIGITAL_A:
        master.clear();
        DEBUG;
        Logging::Interrupter<std::ofstream>("/usd/auton.txt").stream << cur_auton << std::endl;
        DEBUG;
        master.print(0, "Saved" + getNth(cur_auton)->name);
        return;
        break;

      case DIGITAL_B:
      case DIGITAL_X:
        master.clear();
        master.print(0, "Exited Auton Selector");
        return;
        break;

      default: break;
    }

    delay(10);
  }
}

// Returns selected Auton as an int
int Auton::get(){
	int auton_num;
  Logging::Interrupter<std::ifstream>("/usd/auton.txt").stream >> auton_num;
  return auton_num;
}

// Runs selected auton
void Auton::run() {getNth(get())->runFunction();}

Auton::Auton(std::string name, std::function<void()> program, E_Reset_Types reset_type):
name{name}, program{program}, reset_type{reset_type} {}

void Auton::runFunction() const {
  if(program){
    auton_log("Running auton %s\n", name);
    program();
  }
  else alert::start("No Auton function for" + name);
}