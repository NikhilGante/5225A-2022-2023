#include "main.h"
#include "Libraries/geometry.hpp"
#include "Libraries/pid.hpp"
#include "Libraries/piston.hpp"
#include "Libraries/printing.hpp"
#include "Libraries/timer.hpp"
#include "Libraries/state.hpp"
#include "Libraries/gui.hpp"
#include "Libraries/task.hpp"

#include "Libraries/logging.hpp"

#include "lift.hpp"
#include "pros/misc.h"
#include "tracking.hpp"
#include "drive.hpp"
#include "config.hpp"


#include "Libraries/motor.hpp"
#include "Libraries/ControllerButtons.hpp"

#include "pros/llemu.hpp"
#include "pros/rtos.h"
#include <fstream>


const GUI* GUI::current_gui = &util_obj;

/* Nathan's Thoughts
why is queuePrintFile not a member function of Queue?
change most #defines to constexpr
replace C macros with C++ equivalents (M_PI) - not super important
std::string instead of const char*
const after type (int const, char const *)
acivate using namespace pros in main.h
try to remove using namespace std if everyone is ok with that
switch variadic functions to variadic templates
make more methods const
*/


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	DEBUG;
	// // log_init();
	// // lcd::initialize();
	// // tracking.g_pos = {31.0, 11.5, 0.0};
	// // // tracking.g_pos = {70.0, 129.5, M_PI};
	// // _Task_ tracking_task("tracking_update_task");
	// // tracking_task.start(trackingUpdate);
  	// // GUI::init();
	// Data::init();
	// ControllerButton::init();

	// _Controller::init();
	// log_init();
	// pros::delay(500);
	// lift.runMachine();

}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */




// // Text Splitter which takes the text and how many characters per line and splits the texts on spaces to match that parameter
vector<string> textSplit(string text, int lineChar){
	vector<string> output;
	int temp;
	int maxChars = lineChar;
	int index = 0;
	int length = text.length();
	for (int i = 0; i < lineChar; i ++) text.push_back(' ');


    while (index < length){
    	temp = -1;
    	for(int i = index+maxChars+1; i >= index+maxChars-10; i--){
    		if (text[i] == ' '){
    			temp = i;
    			break;
    		}
    	} 
    	if (temp>=0) output.push_back(text.substr(index, temp-index));
    	index = temp+1;
    	//cout << index << endl;

    }
	return output;

}


struct auton {
	static int curAuton;
	string name;
	std::function<void()> function;
	static vector<auton*> array;
	int x; int y;

	auton (string name, std::function<void()> function):
	name(name), function(function){
		array.push_back(this);
	}
	static void increase(){

		if (curAuton == array.size() - 1) curAuton = 0;
		else curAuton++;
	}
	static void decrease() {
		if (curAuton == 0) curAuton = array.size() - 1;
		else curAuton--;
	}
	static auton* GetCurAuton(){return array[curAuton];}

	static void run(){
		GetCurAuton()->function();

		if (!usd::is_installed()) alert::start("No SD Card!");
		else{
			std::ofstream auton_file ("/usd/auton.txt");
			auton_file << GetCurAuton()->name << endl;
			auton_file.close();
			alert::start(term_colours::NOTIF, "Saved");
		}
	}

	
	
};

vector<auton*> auton::array{};
int auton::curAuton = 0;

void autonNumero1(){
	pros::Distance distance_sensor1(10);
	pros::Distance distance_sensor2(11);
	cout << distance_sensor1.get() << endl;
	cout << distance_sensor2.get() << endl;


} void autonNumero2(){
	//fputs("Auton 2", usd_file_write);
} void autonNumero3(){
	//fputs("Auton 3", usd_file_write);
}
auton a("test1", autonNumero1);
auton b("test2", autonNumero2);
auton c("test3", autonNumero3);
/*
Somehow, the library gets currupted, which means if your trying to use hot linking, it doesn't work.
If youre not using hot linking, it will work because it only downloads the parts of the library that works.
*/








void opcontrol() {




	master.clear();
	while (!master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
		if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_RIGHT)) auton::increase();
		else if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_LEFT)) auton::decrease();
		cout << auton::GetCurAuton()->name << endl; // This has to be printed on the screen

		//cout << auton::GetCurAuton()->name.c_str() << endl;
		master.print(0, 0, "%s", auton::GetCurAuton()->name.c_str());

		pros::delay(50);
	}

	auton::run(); 
	




	/*while (1){
		if (ButtonA.doubleClick()) printf("ButtonA Double Click\n");
		if (ButtonB.doubleClick()) printf("ButtonB Double Click\n");
		if (ButtonY.doubleClick()) printf("ButtonY Double Click\n");
		if (ButtonX.doubleClick()) printf("ButtonX Double Click\n");
		if (ButtonA.holdClick()) printf("ButtonA Hold Click\n");
		if (ButtonB.holdClick()) printf("ButtonB Hold Click\n");
		if (ButtonY.holdClick()) printf("ButtonY Hold Click\n");
		if (ButtonX.holdClick()) printf("ButtonX Hold Click\n");
		pros::delay(100);
	}*/




	


	
}