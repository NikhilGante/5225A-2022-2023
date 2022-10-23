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
#include "util.hpp"
#include <fstream>
#include <iterator>


//#include "Libraries/timer copy.hpp"



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




/* Things to work on

#define vars should always be capital with _
Distance sensor class which has name and port for each object
Comment auton selector 


*/


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	DEBUG;
	delay(500);
	// // log_init();
	// // lcd::initialize();
	// // tracking.g_pos = {31.0, 11.5, 0.0};
	// // // tracking.g_pos = {70.0, 129.5, M_PI};
	// // _Task_ tracking_task("tracking_update_task");
	// // tracking_task.start(trackingUpdate);
	// GUI::init();
	Data::init();
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





/*
Somehow, the library gets currupted, which means if your trying to use hot linking, it doesn't work.
If youre not using hot linking, it will work because it only downloads the parts of the library that works.
*/




#define fieldMaxMM 1134
#define distanceOffsetMM 23
#define distanceSensorOffset 20

pros::Distance distance_sensor1(20); // Left Sensor
class auton{
public:
	std::string name;
	double x;
	double y;
	double a;
	bool distanceX = false;
	bool distanceY = false;
	std::function<void(void)> autonProgram;

	

	static int curAuton;
	static vector<auton*> array;


	auton(std::string name, double autonX, double autonY, double autonA, std::function<void(void)> autonProgram): 
	name(name), 
	x(autonX), 
	y(autonY), 
	a(autonA), 
	autonProgram(autonProgram)
	{
		array.push_back(this);
		if (x<0) distanceX = true;
		if (y<0) distanceY = true;
	}

	// Methods ---------------------------------------------------------
	static auton* GetCurAuton(){return array[curAuton];}
	static void increase(){
		if (curAuton == array.size() - 1) curAuton = 0;
		else curAuton++;
	}
	static void decrease() {
		if (curAuton == 0) curAuton = array.size() - 1;
		else curAuton--;
	}
	static double distance(){
		return distance_sensor1.get()/25.4;
	}
	static void calculateAutonValues(){
		if (GetCurAuton()->distanceX) GetCurAuton()->x = distance();
		if (GetCurAuton()->distanceY) GetCurAuton()->y = distance(); 
	}
	static void selectAuton(){
		master.clear();
		delay(100);
		while (!master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
			if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_UP)) decrease();
			else if (master.get_digital_new_press(E_CONTROLLER_DIGITAL_DOWN)) increase();

			//cout << GetCurAuton()->name << endl;
			master.print(0, 0, "%s           ", GetCurAuton()->name.c_str());

			pros::delay(50);
		}
	}
	static void setAutonValues(){
		master.clear();
		delay(100);
		while (!master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
			master.print(0, 0, "%lf", GetCurAuton()->x);
			master.print(1, 0, "%lf", GetCurAuton()->y);
			master.print(2, 0, "%lf", GetCurAuton()->a);
			if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_X)) calculateAutonValues();
			delay(150);
		}
		resetTracking();
	}
	static void resetTracking(){
		delay(1);
	}
	// ------------------------------ must show Tracking VALUES ------------------------------
	static void adjustRobot(){
		master.clear();
		delay(100);
		while (!master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
			master.print(0, 0, "%lf", 0);
			delay(50);
			master.print(1, 0, "%lf", 0);
			delay(50);
			master.print(2, 0, "%lf", 0);
			delay(50);
		}
	}
	// ------------------------------ Save must save Tracking VALUES ------------------------------
	static void save(){
		if (!usd::is_installed()) alert::start("No SD Card!");
		else{
			std::ofstream auton_file ("/usd/auton.txt");
			auton_file << curAuton << endl;
			auton_file << GetCurAuton()->x << endl;
			auton_file << GetCurAuton()->y << endl;
			auton_file << GetCurAuton()->a << endl;
			auton_file << GetCurAuton()->name << endl;
			auton_file.close();
			alert::start(term_colours::NOTIF, "Saved");
		}
	}

	static void readValues(){
		while (!master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_A)) {
			master.print(0, 0, "%s Download Valules?", GetCurAuton()->name);
			delay(50);
		}
		if (!usd::is_installed()) alert::start("No SD Card!");
		else{
			std::ifstream auton_file ("/usd/auton.txt");
			auton_file >> curAuton;
			auton_file >> GetCurAuton()->x;
			auton_file >> GetCurAuton()->y;
			auton_file >> GetCurAuton()->a;
			auton_file.close();
			alert::start(term_colours::NOTIF, "Values Downloaded");
		}
	}
	static void confirmValues(){
		master.clear();
		while (!master.get_digital_new_press(E_CONTROLLER_DIGITAL_A)){
			cout << "Confirmig values" << ' ' << GetCurAuton()->x << ' ' << GetCurAuton()->y << ' ' << GetCurAuton()->a << endl;
			master.print(0, 0, "%lf            ", GetCurAuton()->x);
			master.print(1, 0, "%lf            ", GetCurAuton()->y);
			master.print(2, 0, "%lf            ", GetCurAuton()->a);
			delay(150);
		}
	}
	static void runAuton(){
		printf("Auton Running");
		GetCurAuton()->autonProgram();
	}


};

vector<auton*> auton::array{};
int auton::curAuton = 0;





// pros::Distance distance_sensor1(19); // Right Sensor
void autonNumeroUno(){

}
void autonNumeroDeux(){
	
}
void autonNumeroThree(){
	
}

auton auton1("AutonName1",-1, 0, 90, autonNumeroUno);
auton auton2("AutonName2",0, -1, 90, autonNumeroDeux);
auton auton3("AutonName3",0, 0, 90, autonNumeroThree);



void opcontrol() {
	cout << "HI" << endl;
	// master.clear();
	// delay(150);
	// master.print(0,  0, "%d", 20);
	// master.print(1, 2, "%d", 20);
	// master.print(2, 0, "%d", 20);
	// delay(150);
	//Program 1
	// auton::selectAuton();
	// auton::setAutonValues();
	// auton::resetTracking();
	// auton::adjustRobot();
	// auton::save();

	// Program 2
	// master.clear();
	// auton::readValues();
	// auton::confirmValues();
	// auton::runAuton();


}

/*
AutonSelector Runs
	User is able to select which auton runs from the list of auton names give



Robot reset
	Robot reset the x y a coords using preset values and the distace sensors
	Robot is resetting tracking here
	User should have option to re reset the values if distance is off


Robot is in free movment
	User adjusts the robot while tracking is on so the x y a coords are constantly updated


Save
	User saves tracking values after looking at them
	These are saved to a sd card

Main program is ran with auton
	The user runs the main program which reads the auton from the sd card
	The program runs the auton using the saved x y a values




Auton struct Vars/Methods:
Static:
	Methods:
		AutonNameSelector
		SetsAutonValues - Should be able to redo
		CalculateAutonValues
		Distance
		ResetTracking
		AdjustRobot - This should run tracking in the background
		Save - Saves values
		ReadAutonValues
		RunAuton
	Vars:
		List of autons
		curAuton
Non Static:
	Vars:
		Name
		X - if a negative number, it will use the distance sensor
		Y - if a negative number, it will use the distance sensor
		A





*/





/*
Test1 - Uploaded main project with usePackage 1
Notes:
	Printed test
	Screen was blank
Test2 - Uploaded main project with usePackage 0
Notes:
	Printed test
	Screen was blank
Test3 - Uploaded main project with usePackage 0 - Deleted code mid Upload
Notes:
	I deleted a simple print command while the main upload was occuring(took 1 minute)
	Printed the command I deleted - Very interesting
	Going to retry editing the file before this main upload
	Everything worked fine
Test4 - Uploaded main project with usePackage 0 - Deleted code mid Upload 
Notes:
	I deleted a simple print command right as I entered the pros mut command
	Printed the command I deleted - Very interesting
	Everything worked fine
Test5 - Uploaded main proejct with usePackage 1 - Deleted code mid upload
	I deleted a simple print command while the main upload was occuring(during the compressing binary)
	Printed the command I deleted
	Everything worked fine
Test6 - Uploaded main proejct with usePackage 1 - Change the file a bunch during upload
	Changed a bunch of stuff during the upload but nothing still happend
Test7 - Before uploading the main project with use package 1, i uploaded a other blank project with use package 0
	Everything worked
Test8 - Before uploading the main project with use package 1, I uploaded a other blank project with use package 1
	I changed nothing and it still worked - I might have also not changed anything to the program therefore uploading the exact same program as before
Test9 - I uploaded a other blank project with use package 1, I uploaded main project with use package 1
	I changed code while upload but nothing happend
	Everything worked
Test10 - I uploaded code from sabrains laptop with usepackage 1. Then I ran my code with usepackage 1 
	Everything worked
	I uploaded it with changing but then i got a common error during download and becase my changes caused errors, I deleted my chages
	When I reran it also changing the file mid download, it still worked


I ran into the issue so I had Jackson upload his code with hotlinking enabled and I uploaded mine with it disabled.
	Mine still failed to work :(

I had jackson upload his code again with hotlinking and he got the same problem I go t------------- Wtf ------------

*/