#include "menu.hpp"
#include "config.hpp"
#include "Devices/controller.hpp"
#include "Libraries/logging.hpp"

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
      auto file = Logging::Interrupter<std::ofstream>("auton");
			file.stream << cur_auton << std::endl;
			master.print(0, "Saved.");
			break;
		}
		delay(10);
	}
}

// Returns selected Auton as an int
int Auton::get(){
  auto file = Logging::Interrupter<std::ifstream>("auton");
	int auton_num;
	file.stream >> auton_num;
  return auton_num;
}

// Runs selected auton
void Auton::run() {getNth(get())->runFunction();}

Auton::Auton(std::string name, std::function<void()> program, E_Reset_Types reset_type): name{name}, program{program}, reset_type{reset_type} {}

void Auton::runFunction() const {
  if(program) program();
  else alert::start("No Auton function for" + name);
}