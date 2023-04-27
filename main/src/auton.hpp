#pragma once
#include  "config.hpp"
#include "tracking.hpp"
using namespace pros;


void moveInches(double target, double max_power = 127, E_Brake_Modes brake_mode = E_Brake_Modes::brake, double end_error = 0.5);


void autonAWP();
void shortAutonAWP();


void autonStack();
void autonStackMoveBack();
void autonStackLong();

void autonLine();
void autonLineShort();

void worldsSkills();


