#include "menu.hpp"
#include "tracking.hpp"
#include "auton.hpp"
#include "stdio.h"
using namespace pros;

array<Auton*, MAX_AUTON_ARR_SIZE> Auton::auton_arr;

int Auton::autons_constructed = 0;

E_Auton_Reset_Types Auton::getResetType(){ // Getter
	return reset_type;
}

int Auton::getAuton(){	// Returns selected Auton as an int
	log_mutex.take();
	ifstream myfile ("/usd/auton.txt", ios::in);
	int auton_num;
	myfile >> auton_num; 
	myfile.close();
	log_mutex.give();
	return auton_num;
}

void Auton::selectAuton(){
	int cur_auton = 0;
	master.clear();
	master.print(0, 0, "%s            ", auton_arr[cur_auton]->name);
	master.print(1, 0, "Up/dn change auton");
	master.print(2, 0, "Press A to save");
	printf("constructed: %d\n", autons_constructed);
	while(true){
		if(master.get_digital_new_press(DIGITAL_UP) && cur_auton < autons_constructed - 1){
			master.print(0, 0, "%s            ", auton_arr[++cur_auton]->name);
		}
		if(master.get_digital_new_press(DIGITAL_DOWN) && cur_auton > 0){
			master.print(0, 0, "%s            ", auton_arr[--cur_auton]->name);
		}
		if(master.get_digital_new_press(DIGITAL_A)){	// Press A to save
			master.clear();
			log_mutex.take();
			ofstream myfile ("/usd/auton.txt", ios::out);
			myfile << cur_auton << endl; 
			myfile.close();
			log_mutex.give();
			master.print(0, 0, "Saved.");
			break;
		}
		delay(10);
	}
	/*
	while(true){
		if(master.get_digital_new_press(DIGITAL_A)){	// Press A to reset
			flattenAgainstWallSync();

			switch(auton_arr[getAuton()]->getResetType()){
				case E_Auton_Reset_Types::home:
					tracking.reset({getDistL(), 9.75, degToRad(0.0)});
					break;

				case E_Auton_Reset_Types::far:
					tracking.reset({131.25, 141-getDistR(), degToRad(-90.0)});
					break;
			}
			moveDrive(0, 0);
			trans_p.setState(HIGH);
			break;
		}
		delay(10);

	}
	*/
	
}
// Runs selected auton
void Auton::runAuton(){
	int auton_num = getAuton();
	auton_arr[auton_num]->run();	// Runs Auton
}

Auton::Auton(const char* name, std::function<void(void)> program, E_Auton_Reset_Types reset_type): name(name), program(program), reset_type(reset_type){
	auton_arr[autons_constructed++] = this;
}

void Auton::run(){
	program();
}