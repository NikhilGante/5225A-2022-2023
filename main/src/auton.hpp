#pragma once
#include  "config.hpp"
#include "tracking.hpp"
using namespace pros;


void moveInches(double target, double max_power = 127, E_Brake_Modes brake_mode = E_Brake_Modes::brake);

void provSkills();
void provSkillsLow();
void worldSkills();


void backupSkills();

void skills1();
void skills2();
void skills3();

void autonStack();
void autonAWP4();
void autonAWP5(); // Attempts Stack

void autonLine();
void autonLine2();  // Goes to roller last


