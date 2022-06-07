#pragma once
#include "main.h"
#include "Libraries/state.hpp"

extern int lift_index;
extern int lift_arr[5];

struct LiftMttParams{
  const char* getName(){
    return "Move to target";
  }
  void handle(){
    // int output = b_lift_pid.compute(b_lift_pot.get_value(), lift_arr[lift_index]);
    // if(abs(output) < 40) output = 40 * sgn(output); // enforces a minimum of 30 power
    // b_lift_m.move(output);
    // if(fabs(b_lift_pid.get_error()) < 10){
    //   printf("Finished move, %d\n", b_lift_pot.get_value());
    //   b_lift_m.move_velocity(0);
    //   return;
    // }
    // delay(10);
  }
  void handleStateChange(size_t prev_state){
    // if(prev_state == GET_ID(aParams)) 
  }
};
