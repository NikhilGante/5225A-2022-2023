#pragma once
#include "main.h"
#include "config.hpp"

#include <fstream>
#include <array>
#include <functional>
using namespace std;
using namespace pros;

#define MAX_AUTON_ARR_SIZE 10

enum class E_Auton_Reset_Types{
	home,
	far
};

class Auton{
	const char* name;
	std::function<void(void)> program;
	E_Auton_Reset_Types reset_type;
	static array<Auton*, MAX_AUTON_ARR_SIZE> auton_arr;
	static int autons_constructed;

public:

	Auton(const char* name, std::function<void(void)> program, E_Auton_Reset_Types reset_type = E_Auton_Reset_Types::home);
	E_Auton_Reset_Types getResetType(); // Getter
	static int getAuton();	// Returns selected Auton as an int
	static void selectAuton();
	static void runAuton();	// Reads auton from SD card and runs it
	void run();
};
