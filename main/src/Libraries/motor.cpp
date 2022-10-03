#include "motor.hpp"


// MOTOR motor1 ("Test1", 1, false, pros::motor_gearset_e::E_MOTOR_GEAR_BLUE, MOTOR::jam::stop);
// MOTOR motor2 ("Test2", 2, false, pros::motor_gearset_e::E_MOTOR_GEAR_GREED, MOTOR::jam::reverse);
// MOTOR motor3 ("Test3", 3, false, pros::motor_gearset_e::E_MOTOR_GEAR_RED, MOTOR::jam::continue);
// MOTOR motor4 ("Test4", 4, false, pros::motor_gearset_e::E_MOTOR_GEAR_BLUE, MOTOR::jam::stop);

MOTOR::MOTOR (std::string name, int port, bool Reverse, pros::motor_gearset_e gearset, MOTOR::jam jamF, int temp_max, int jammed):
pros::Motor(port, gearset, Reverse, pros::E_MOTOR_ENCODER_DEGREES),
name(name),
temp_max(temp_max),
jammed(jammed)
{
    array.push_back(this); // Adds pointer to motor array. This pointer points to this motor class.
    jamFunction = MOTOR::jam(jamF);
}


// Checks the tempature. If tempature is beyond or equal to temp_max, it will turn off the motor and log it. 
void MOTOR::TempatureCheck(){
    if (get_temperature() >= this->temp_max){
        move(0);
        printf("Motor %s surpassed the tempature %d\n", this->name.c_str(), this->temp_max);
        // I want to add a alert on the controller screen.
    }
}

// Sees if the motor efficieny dips below the vaiable jammed. Returns true if jammed, false if not jammed.
bool MOTOR::IsJammed(){
    if (fabs((jammed/100.0)*get_target_velocity()) > fabs(get_actual_velocity())){
        JamFunction();
        printf("Motor %s has gone beneath the efficieny threshold %d\n", this->name.c_str(), this->jammed);
        return true;
    }
    printf("Motor %s has not gone beneath the efficieny threshold %d\n", this->name.c_str(), this->jammed);
    return false;
}


void MOTOR::JamCheck() {
    for (int i = 0; i < array.size(); i++){
        if (array[i]->IsJammed()) printf("%s was jammed", array[i]->name.c_str());
        else printf("%s was not jammed", array[i]->name.c_str());
    }	
}


// Extend of pros motor move taking the parameter voltage. It also logs the time of the function, the previous voltage, and current voltage.
void MOTOR::move(std::int32_t voltage){
    printf("Motor Move command was run at %.2f from voltage %d to %d \n", pros::c::millis()/1000.0, this->prev_voltage, voltage);
    pros::Motor::move(voltage);
    this->prev_voltage = voltage;
}

// Decides what to do when the motor is jammed, using the function jamFunction which is setup when constructing the class.
void MOTOR::JamFunction(){
    switch(this->jamFunction){
        case 0:
            this->move(0);
            break;
        case 1:
            this->move(-(this->prev_voltage));
            break;
        case 2:
            break;
    }
}

// Changes the action to do if the motor gets Jammed.
void MOTOR::changeJamFunction(MOTOR::jam jamF){
    this->jamFunction = jamF;
}

// Intializes the list of motors.
vector<MOTOR*> MOTOR::array{};