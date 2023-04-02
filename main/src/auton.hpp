#pragma once
#include  "config.hpp"
#include "tracking.hpp"
using namespace pros;


void moveInches(double target, double max_power = 127, E_Brake_Modes brake_mode = E_Brake_Modes::brake);

void autonStack();
void autonAWP();
void autonLine();


