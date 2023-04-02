#pragma once
#include "config.hpp"
#include "tracking.hpp"
#include "menu.hpp"


namespace UtilProgram {
	struct Test {
		function<void()> test_function;
		std::string name;

		Test(function<void()> test_function, std::string name): test_function(test_function), name(name) {}

		void run_function() {
			test_function();
		}
	};

	struct TestingSubsection{
		std::vector<Test> test;
		std::string name;

		TestingSubsection(std::vector<Test> tests, std::string name) : test(tests), name(name) {}

		void print(int index){
			master.print(2, 0, "%s%s%s", std::string(9-test[(index)%(test.size())].name.length()/2, ' ').c_str(), test[(index-1)%(test.size())].name.c_str(), std::string(9-test[(index)%(test.size())].name.length()/2, ' ').c_str());
			master.print(1, 0, "%s%s%s", std::string(9-test[(index)%(test.size())].name.length()/2, '-').c_str(), test[(index)%(test.size())].name.c_str(), std::string(9-test[(index)%(test.size())].name.length()/2, '-').c_str());
			master.print(0, 0, "%s%s%s", std::string(9-test[(index)%(test.size())].name.length()/2, ' ').c_str(), test[(index+1)%(test.size())].name.c_str(), std::string(9-test[(index)%(test.size())].name.length()/2, ' ').c_str());

			cout << (index)%(test.size()) << endl;
		}

		void selectTest(){
			std::string input; 
			int index = 1;

			print(index);

			while (true){ // not b is press
				
				if (master.get_digital_new_press(DIGITAL_UP)){
					index++;
					print(index);
				} else if (master.get_digital_new_press(DIGITAL_DOWN)){
					index--;
					if (index == 0) index =test.size()-1;
					print(index);
				} else if (master.get_digital_new_press(DIGITAL_A)){
					test[index%test.size()].run_function();
				} else if (master.get_digital_new_press(DIGITAL_B)){
					break;
				}

				delay(50);
			}
		}
	};

	void turn45() {
		turnToAngleSync(45);
	}
	void turn90() {
		turnToAngleSync(90);
	}
	void turn15() {
		turnToAngleSync(15);
	}
	void turnX(){
		int degrees = 25;
		master.clear();
		master.print(0, 0, "Inches: %d", degrees);
		while (true){
			if (master.get_digital_new_press(DIGITAL_UP)) master.print(0, 0, "Inches: %d", degrees += 1);
			else if (master.get_digital_new_press(DIGITAL_DOWN)) master.print(0, 0, "Inches: %d", degrees -= 1);
			else if (master.get_digital_new_press(DIGITAL_LEFT)) master.print(0, 0, "Inches: %d", degrees -= 5);
			else if (master.get_digital_new_press(DIGITAL_RIGHT)) master.print(0, 0, "Inches: %d", degrees += 5);
			else if (master.get_digital_new_press(DIGITAL_A)) {moveForwardSync(degrees); break;}
			else if (master.get_digital_new_press(DIGITAL_B)) break;

			delay(50);
		}

		turnToAngleSync(degrees);

		
	}


	void move70() {
		moveForwardSync(70);
	}
	void move24() {
		moveForwardSync(24);
	}
	void move10() {
		moveForwardSync(10);
	}
	void moveX(){
		int inches = 25;
		master.clear();
		master.print(0, 0, "Inches: %d", inches);
		while (true){
			if (master.get_digital_new_press(DIGITAL_UP)) {inches++; master.print(0, 0, "Inches: %d", inches);}
			else if (master.get_digital_new_press(DIGITAL_DOWN)) {inches--; master.print(0, 0, "Inches: %d", inches);}
			else if (master.get_digital_new_press(DIGITAL_LEFT)) {inches -= 5; master.print(0, 0, "Inches: %d", inches);}
			else if (master.get_digital_new_press(DIGITAL_RIGHT)) {inches += 5; master.print(0, 0, "Inches: %d", inches);}
			else if (master.get_digital_new_press(DIGITAL_A)) {moveForwardSync(inches); break;}
			else if (master.get_digital_new_press(DIGITAL_B)) break;

			delay(50);
		}

		
	}
    

	void run() {
		TestingSubsection turns{{{turn45, "Turn45"}, {turn90, "Turn90"}, {turn15, "turn15"}, {turnX, "turnX"}}, "Turns"};
		TestingSubsection moves{{{move70, "move70"}, {move24, "move24"}, {move10, "move10"}, {moveX, "moveX"}}, "Moves"};			

		std::string input;
		int index = 1;
		master.clear();

		TestingSubsection test_sub[] = {turns, moves};

		#define print_util_options \
        master.print(2, 0, "%s%s%s", std::string(9-test_sub[(index-1)%(sizeof(test_sub) / sizeof(test_sub[0]))].name.length()/2, ' ').c_str(), test_sub[(index-1)%(sizeof(test_sub) / sizeof(test_sub[0]))].name.c_str(), std::string(9-test_sub[(index-1)%(sizeof(test_sub) / sizeof(test_sub[0]))].name.length()/2, ' ').c_str()); \
        master.print(1, 0, "%s%s%s",  std::string(9-test_sub[(index)%(sizeof(test_sub) / sizeof(test_sub[0]))].name.length()/2, '-').c_str(), test_sub[(index  )%(sizeof(test_sub) / sizeof(test_sub[0]))].name.c_str(), std::string(9-test_sub[(index  )%(sizeof(test_sub) / sizeof(test_sub[0]))].name.length()/2, '-').c_str()); \
        master.print(0, 0, "%s%s%s", std::string(9-test_sub[(index+1)%(sizeof(test_sub) / sizeof(test_sub[0]))].name.length()/2, ' ').c_str(), test_sub[(index+1)%(sizeof(test_sub) / sizeof(test_sub[0]))].name.c_str(), std::string(9-test_sub[(index+1)%(sizeof(test_sub) / sizeof(test_sub[0]))].name.length()/2, ' ').c_str())

        print_util_options;

		while (true){

			if (master.get_digital_new_press(DIGITAL_UP)){
				index++;
				print_util_options;
			} else if (master.get_digital_new_press(DIGITAL_DOWN)){
                if (index == 1) index = (sizeof(test_sub) / sizeof(test_sub[0]));
			    else index--;

				print_util_options;

			} else if (master.get_digital_new_press(DIGITAL_A)){
				test_sub[index%(sizeof(test_sub) / sizeof(test_sub[0]))].selectTest();
				print_util_options;
			} else if (master.get_digital_new_press(DIGITAL_B)){
                break;
            }

			printf("Below: %d Center: %d Up: %d\n", (index-1)%(sizeof(test_sub) / sizeof(test_sub[0])), (index)%(sizeof(test_sub) / sizeof(test_sub[0])), (index+1)%(sizeof(test_sub) / sizeof(test_sub[0])));


			delay(50);
		}
	}

}

// Forward Declare because I am lazy to move it up here
void match_tests();


enum class E_Util_Options{
	UTIL,
	SELECT,
	TEST,
	DRIVER_SKILLS,
	AUTON
};
array<std::string, 5> util_names {"UTIL", "SELECT AUTON", "TEST", "DRIVER_SKILLS", "RUN AUTON"};

void print_options(int index){
	master.print(0, 0, "%s%s%s", std::string(9-util_names[(index-1)%util_names.size()].length()/2, ' ').c_str(), util_names[(index-1)%util_names.size()].c_str(), std::string(9-util_names[(index-1)%util_names.size()].length()/2, ' ').c_str());
	master.print(1, 0, "%s%s%s", std::string(9-util_names[index%util_names.size()].length()/2, '-').c_str(), util_names[index%util_names.size()].c_str(), std::string(9-util_names[index%util_names.size()].length()/2, '-').c_str());
	master.print(2, 0, "%s%s%s", std::string(9-util_names[(index+1)%util_names.size()].length()/2, ' ').c_str(), util_names[(index+1)%util_names.size()].c_str(), std::string(9-util_names[(index+1)%util_names.size()].length()/2, ' ').c_str());
}

// This is not used for matches. This is used to testing util programs, autons, driver_practice, etc.
void util_selector(){
	Timer selector_timer{"selector_timer"};

	master.clear();

	std::string input; 
	int index = 1;

	print_options(index);
	
	while (true){			
		if (master.get_digital_new_press(DIGITAL_UP)){
			if (index == 1) index = util_names.size();
			else index--;
			print_options(index);
		} else if (master.get_digital_new_press(DIGITAL_DOWN)){
			index++;
			print_options(index);
		} else if (master.get_digital_new_press(DIGITAL_A)){

			switch(static_cast<E_Util_Options>(index%util_names.size())){
				case E_Util_Options::SELECT:
					Auton::selectAuton();
					print_options(index);
					break;
				case E_Util_Options::DRIVER_SKILLS:
					driverPractice();
					break;
				case E_Util_Options::UTIL:
					UtilProgram::run();
					break;
				case E_Util_Options::TEST:
                    match_tests();
					break;
				case E_Util_Options::AUTON:
					return;
					break;
			}

            print_options(index);
		}

		delay(50);
	}
}




void match_tests(){
    // First checks gyro..
    master.clear();
    lcd::print(7, " Gyro_drift check. Do not touch");
    master.print(0, 0, "Drift Check");
    delay(1500);
    lcd::print(7, "%sChecking gyro", std::string(16-7, ' '));
    master.print(0, 0, "Checking gyro");
    std::string dots = "";
    Timer test_check{"test_check"};
    int drift_count = 0;

    while (test_check.getTime() < 2000){
        if (test_check.getTime()%350 < 16) {
            dots = std::string((dots.length()+1)%4, '.');
            lcd::print(7, "%sChecking gyro%s",std::string((16-7)-int(dots.length()/2), ' '), dots);
            master.print(0, 0, "Drift Check%s", dots.c_str());
        }

        if(fabs(radToDeg(tracking.g_vel.a)) > 0.01){
            drift_count++;
            if(drift_count > 10){
                master.print(0, 0, "GYRO DRIFT");
                master.rumble("-----");
                screen::set_pen(COLOR_RED);
                screen::fill_rect(0, 0, 480, 240);
                lcd::print(7, "GYRO IS DRIFTING");
                delay(1000);
                screen_erase();
                return;
            }
        }
        
        
        delay(15);
    }
    lcd::print(7, "          GYRO IS FINE");
    delay(1500);
    lcd::print(7, "      Please move the robot");
    master.print(0, 0, "Move the robot");
    test_check.reset();
    WAIT_UNTIL(tracking.r_vel > 0.1 || test_check.getTime() > 1500);
    if (test_check.getTime() > 1500){
        master.print(0, 0, "WHEEL's ZEROED");
        master.rumble("-----");
        screen::set_pen(COLOR_RED);
        screen::fill_rect(0, 0, 480, 240);
        lcd::print(7, "WHEEL IS ZEROED");
        delay(2000);
        screen_erase();
        return;
    } else {
        test_check.reset();
        WAIT_UNTIL(tracking.g_vel.a > 0.1|| test_check.getTime() > 1500);
        if (test_check.getTime() > 1500){
            master.print(0, 0, "GYRO's ZEROED");
            master.rumble("-----");

            screen::set_pen(COLOR_RED);
            screen::fill_rect(0, 0, 480, 240);
            lcd::print(7, "GYRO IS ZEROED");
            delay(2000);
            screen_erase();
            return;
        }
    }
    
}
enum class E_Program_Options{
	SELECT,
	DRIVER_SKILLS,
    MATCH,
    TEST,
};
array<std::string, 4> program_names {"SELECT", "DRIVER_SKILLS", "MATCH", "TEST"};

E_Program_Options program_state = E_Program_Options::MATCH;

bool handle_program(E_Program_Options program_type){
    printf("HANDING PROGRAM\n");
    switch(program_type){
        case E_Program_Options::SELECT:
            Auton::selectAutonLcd();
            lcd::print(7, "         AUTON SELECTED");
            master.rumble(".");
            delay(1500);
            return false;
            break;
        case E_Program_Options::TEST:
            match_tests();
            return false;
            break;
        case E_Program_Options::MATCH:
            lcd::print(7, "         READY FOR MATCH");
            log("SELECTED MATCH");
            program_state = program_type;
            return true;
            break;
        case E_Program_Options::DRIVER_SKILLS:
            lcd::print(7, "         READY FOR DRIVER");
            log("SELECTED DRIVER");
            program_state = program_type;
            return true;
            break;
    }
    return false;
}

// This is used for matches(on the screen). Able to reselect auton, run match, or driverskills/prog_skills
void program_selector(){
    int index = 1, cur = 0, prev = 0;
    
    lcd::print(7, "%s%s%s",  std::string(16-program_names[index].length()/2, ' '), program_names[index], std::string(16-program_names[index].length()/2, ' '));
    
    while (true){
        cur = pros::lcd::read_buttons();

        if (prev != cur){
            if (cur == 1){
                index++;
            } else if (cur == 4){
                if (index == 0) index += program_names.size();
                index--;

            } else if (cur == 2){
                WAIT_UNTIL(pros::lcd::read_buttons() != 2);
                if (handle_program(static_cast<E_Program_Options>(index))) return;
            }
            index %=  program_names.size();
            lcd::print(7, "%s%s%s",  std::string(16-program_names[index].length()/2, ' '), program_names[index], std::string(16-program_names[index].length()/2, ' '));

        }

        prev = pros::lcd::read_buttons();

        delay(50);
    }
}
