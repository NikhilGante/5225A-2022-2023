#pragma once
#include "main.h"
#include "Libraries/counter.hpp"

class Auton: public Counter<Auton, 10>{
  private:
    std::string name;
    std::function<void()> program;
    
    void runFunction() const;

  public:
    Auton(std::string name, std::function<void()> program);
    static void select();
    static void run();	// Reads auton from SD card and runs it
};
