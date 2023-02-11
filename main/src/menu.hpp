#pragma once
#include "main.h"
#include "Libraries/counter.hpp"

class Auton: public Counter<Auton, 10>{
  public:

    enum class E_Reset_Types{
      home,
      far
    };

  private:
	  E_Reset_Types reset_type;
    std::string name;
    std::function<void()> program;
    
    void runFunction() const;

  public:
    Auton(std::string name, std::function<void()> program, E_Reset_Types reset_type = E_Reset_Types::home);
    static void select();
    static void run();	// Reads auton from SD card and runs it
	  static int get();	// Returns selected Auton as an int
	  E_Reset_Types getResetType() const;	// Returns selected Auton as an int
};
