#pragma once
#include "main.h"
#include "Libraries/tracker.hpp"

class Auton: public ObjectTracker<Auton>{
  public:
    enum class E_Reset_Types{
      home,
      far
    };

  // private:
    E_Reset_Types reset_type;
    std::string name;
    std::function<void()> program;
    
    void runFunction() const;

  public:
    Auton(std::string name, std::function<void()> program, E_Reset_Types reset_type = E_Reset_Types::home);
    static void select();
    static void run();	// Reads auton from SD card and runs it
    static int get();	// Returns selected Auton as an int
    // static std::vector<Auton*> const & getList() {return logs;}
    // static Auton* getNth(std::size_t n) {return getList()[n];}
	  E_Reset_Types getResetType() const;	// Returns selected Auton as an int
};
