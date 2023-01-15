#pragma once
#include "main.h"
#include "config.hpp"

#include <fstream>
#include <array>
#include <functional>
using namespace std;
using namespace pros;

static constexpr int MAX_AUTON_ARR_SIZE = 10;

class Auton{
	const char* name;
	std::function<void(void)> program;
	static array<Auton*, MAX_AUTON_ARR_SIZE> autonArr;
	static int autons_constructed;

public:

	Auton(const char* name, std::function<void(void)> program);
	static void selectAuton();
	static void runAuton();	// Reads auton from SD card and runs it
	void run();
};
