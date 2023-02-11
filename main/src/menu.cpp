#include "menu.hpp"
#include "config.hpp"
#include "Devices/controller.hpp"

#include <fstream>

Auton::E_Reset_Types Auton::getResetType() const {return reset_type;} // Getter

void Auton::select(){
	int cur_auton = 0;
	master.clear();
	master.printScroll("%s", getNth(cur_auton)->name);
	master.printScroll("Up/dn change auton");
	master.printScroll("Press A to save");
	auton_log("constructed: %d", getCount());
	while(true){
		if(master.getNewDigital(DIGITAL_UP) && cur_auton < getCount() - 1) master.print(0, "%s", getNth(++cur_auton)->name);
		if(master.getNewDigital(DIGITAL_DOWN) && cur_auton > 0) master.print(0, "%s", getNth(--cur_auton)->name);
		if(master.getNewDigital(DIGITAL_A)){	// Press A to save
			master.clear();
      Logging::pause();
			std::ofstream myfile ("/usd/auton.txt", std::ofstream::out);
			myfile << cur_auton << std::endl;
      Logging::restart();
			master.print(0, "Saved.");
			break;
		}
		delay(10);
	}
}

// Returns selected Auton as an int
int Auton::get(){
	std::ifstream myfile ("/usd/auton.txt", std::ifstream::in);
	int auton_num;
	myfile >> auton_num;
  return auton_num;
}

// Runs selected auton
void Auton::run() {getNth(get())->runFunction();}

Auton::Auton(std::string name, std::function<void()> program, E_Reset_Types reset_type): name{name}, program{program}, reset_type{reset_type} {}

void Auton::runFunction() const {
  if(program) program();
  else alert::start("No Auton function for" + name);
}