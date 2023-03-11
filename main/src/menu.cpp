#include "menu.hpp"
#include "config.hpp"
#include "Devices/controller.hpp"

#include <fstream>

Auton::Auton(std::string name, std::function<void()> program, E_Reset_Types reset_type):
ObjectTracker{name + " Auton"}, name{name}, program{program}, reset_type{reset_type} {}

// Runs selected auton
void Auton::run() {getNth(get())->runFunction();}
Auton::E_Reset_Types Auton::getResetType() const {return reset_type;} // Getter

// Returns selected Auton as an int
int Auton::get(){
	int auton_num;
  auto file = Logging::Interrupter<std::ifstream>("/usd/auton.txt");
  if(file.stream.is_open()) file.stream >> auton_num;
  else auton_num = -1;
  return auton_num;
}

void Auton::runFunction() const {
  if(program){
    auton_log("Running auton %s\n", name);
    program();
  }
  else alert::start("No Auton function for" + name);
}

void Auton::select(){
	int cur_auton = get();
  if(cur_auton == -1){
    alert::start(3000, "Cannot Run Auton Selector without SD");
    return;
  }
	master.clear();
	master.print(0, getNth(cur_auton) ? getNth(cur_auton)->name : "No Auton " + std::to_string(cur_auton));
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
        Logging::Interrupter<std::ofstream>("/usd/auton.txt").stream << cur_auton << std::endl;
        master.print(0, "Saved " + getNth(cur_auton)->name);
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