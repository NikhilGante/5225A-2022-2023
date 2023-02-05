#include "menu.hpp"
#include "config.hpp"
#include "Libraries/controller.hpp"

#include <fstream>

void Auton::select(){
	int cur_auton = 0;
	master.clear();
	master.print(0, "%s            ", getNth(cur_auton)->name);
	master.print(1, "Up/dn change auton");
	master.print(2, "Press A to save");
	auton_log("constructed: %d", getCount());
	while(true){
		if(master.get_digital_new_press(DIGITAL_UP) && cur_auton < getCount() - 1){
			master.print(0, "%s            ", getNth(++cur_auton)->name);
		}
		if(master.get_digital_new_press(DIGITAL_DOWN) && cur_auton > 0){
			master.print(0, "%s            ", getNth(--cur_auton)->name);
		}
		if(master.get_digital_new_press(DIGITAL_A)){	// Press A to save
			master.clear();
			std::ofstream myfile ("/usd/auton.txt", std::ofstream::out);
			myfile << cur_auton << std::endl;
			master.print(0, "Saved.");
			break;
		}
		delay(10);
	}
}
// Runs selected auton
void Auton::run(){
	std::ifstream myfile ("/usd/auton.txt", std::ifstream::in);
	int auton_num;
	myfile >> auton_num; 
	getNth(auton_num)->runFunction();	//Runs Auton
}

Auton::Auton(std::string name, std::function<void()> program): name(name), program(program) {}

void Auton::runFunction() const{
  if(program) program();
  else alert::start("No Auton function for" + name);
}