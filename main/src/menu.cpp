#include "menu.hpp"
#include "config.hpp"
#include "util.hpp"
#include "Devices/controller.hpp"
#include "Libraries/logging.hpp"

std::vector<Auton*> Auton::logs{};

Auton::E_Reset_Types Auton::getResetType() const {return reset_type;} // Getter

void Auton::select(){
	int cur_auton = 0;
	master.clear();
	master.printScroll("%s", getNth(cur_auton) ? getNth(cur_auton)->name : "Null Auton*");
	master.printScroll("Up/dn change auton");
	master.printScroll("Press A to save");
	auton_log("Constructed: %d", getCount());
	while(true){
		if(master.getNewDigital(DIGITAL_UP) && cur_auton < getCount() - 1) master.print(0, "%s", getNth(++cur_auton)->name);
		if(master.getNewDigital(DIGITAL_DOWN) && cur_auton > 0) master.print(0, "%s", getNth(--cur_auton)->name);
		if(master.getNewDigital(DIGITAL_A)){	// Press A to save
			master.clear();
      Logging::Interrupter<std::ofstream>("/usd/auton.txt").stream << cur_auton << std::endl;
			master.print(0, "Saved.");
			break;
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
name{name}, program{program}, reset_type{reset_type} {
  logs.push_back(this); //! Fix the issue with ObjectTracker
  for(auto auton: Auton::getList()) std::cout << auton->name << std::endl;
}

void Auton::runFunction() const {
  if(program){
    auton_log("Running auton %s\n", name);
    program();
  }
  else alert::start("No Auton function for" + name);
}