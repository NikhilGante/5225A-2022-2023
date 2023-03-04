#pragma once
#include  "config.hpp"
#include "tracking.hpp"
using namespace pros;


void moveInches(double target, double max_power = 127, E_Brake_Modes brake_mode = E_Brake_Modes::brake);

void fullSkills();
void provSkills();
void provSkillsLow();


void backupSkills();

void skills1();
void skills2();
void skills3();

void autonStack();
void autonAWP();
void autonLine();

