#include "lift.hpp"
#include "config.hpp"
#include "Libraries/pid.hpp"

PID b_lift_pid = PID(2.0,0.0,0.0,0.0);
int lift_index = 0;
int lift_arr[5] = {1185, 1400, 1900, 2170, 2775};


Subsystem<LIFT_STATE_TYPES> lift("Lift", LiftMTTParams{100});

// Lift move to target state

LiftMTTParams::LiftMTTParams(int target, int max_power) : target(target), max_power(max_power){}

const char* LiftMTTParams::getName(){
  return "LiftMoveToTarget";
}
void LiftMTTParams::handle(){
  while(true){
    int output = b_lift_pid.compute(b_lift_m.get_position(), target);
    // if(abs(output) < 5) output = 5 * sgn(output); // enforces a minimum of 5 power
    if(abs(output) > max_power) output = max_power * sgn(output); // ensures no more than max_power is outputted

    b_lift_m.move(output);
    if(fabs(b_lift_pid.get_error()) < 10){
      printf("Finished move, %lf\n", b_lift_m.get_position());
      lift.change_state(LiftIdleParams{});
    }
    _Task_::delay(10);
  }
}
void LiftMTTParams::handleStateChange(LIFT_STATE_TYPES_VARIANT prev_state){
  if(std::get_if<LiftIdleParams>(&prev_state)){
    // prev state was idle
  }
}

// Lift idle state
const char* LiftIdleParams::getName(){
  return "LiftIdle";
}
void LiftIdleParams::handle(){}
void LiftIdleParams::handleStateChange(LIFT_STATE_TYPES_VARIANT prev_state){
  b_lift_m.move_velocity(0);
}


