#pragma once
#include "main.h"
#include "config.hpp"

#include <functional>

static constexpr int MAX_AUTON_ARR_SIZE = 10;

class Auton{
  private:
    std::string name;
    std::function<void()> program;
    static std::array<Auton*, MAX_AUTON_ARR_SIZE> autonArr;
    static int autons_constructed;
    
    void runFunction() const;

  public:
    Auton(std::string name, std::function<void()> program);
    static void select();
    static void run();	// Reads auton from SD card and runs it
};
