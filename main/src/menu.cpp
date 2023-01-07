#include "menu.hpp"
#include "stdio.h"
using namespace pros;

array<Auton*, MAX_AUTON_ARR_SIZE> Auton::autonArr;

int Auton::autons_constructed = 0;

void Auton::selectAuton(){
	int cur_auton = 0;
	master.clear();
	master.print(0, 0, "%s            ", autonArr[cur_auton]->name);
	master.print(1, 0, "Up/dn change auton");
	master.print(2, 0, "Press A to save");
	printf("constructed: %d\n", autons_constructed);
	while(true){
		if(master.get_digital_new_press(DIGITAL_UP) && cur_auton < autons_constructed - 1){
			master.print(0, 0, "%s            ", autonArr[++cur_auton]->name);
		}
		if(master.get_digital_new_press(DIGITAL_DOWN) && cur_auton > 0){
			master.print(0, 0, "%s            ", autonArr[--cur_auton]->name);
		}
		if(master.get_digital_new_press(DIGITAL_A)){	// Press A to save
			master.clear();
			ofstream myfile ("/usd/auton.txt", ios::out);
			myfile << cur_auton << endl; 
			myfile.close();
			master.print(0, 0, "Saved.");
			break;
		}
		delay(10);
	}

}

void Auton::runAuton(){
	ifstream myfile ("/usd/auton.txt", ios::in);
	int auton_num;
	myfile >> auton_num; 
	myfile.close();
	autonArr[auton_num]->run();	// Runs Auton
}

Auton::Auton(const char* name, std::function<void(void)> program): name(name), program(program){
	autonArr[autons_constructed++] = this;
}

void Auton::run(){
	program();
}