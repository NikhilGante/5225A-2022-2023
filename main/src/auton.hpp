#pragma once

double getDistL();  // Gets tracking centre's position from wall on left
double getDistR();  // Gets tracking centre's position from wall on right

void moveInches(double target, double max_power = 127);

void fullSkills();
void provSkills();

void skills1();
void skills2();
void skills3();

void autonStack();
void autonAWP();
void autonLine();

