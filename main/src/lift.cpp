#include "lift.hpp"
#include "util.hpp"
#include "config.hpp"
#include "Libraries/pid.hpp"

PID b_lift_pid = PID(0.5,0.0,0.0,0.0);
int lift_index = 0;
int lift_arr[5] = {1185, 1400, 1900, 2170, 2775};


Machine<LIFT_STATE_TYPES> lift("Lift", LiftMTTParams{100});

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
    if(fabs(b_lift_pid.getError()) < 10){
      printf("Finished move, %lf\n", b_lift_m.get_position());
      lift.changeState(LiftIdleParams{});
    }
    _Task_::delay(10);
  }
}
void LiftMTTParams::handleStateChange(LIFT_STATE_TYPES_VARIANT prev_state){
  lcd::print(1, "target: %d", lift_arr[lift_index]);

  if(std::get_if<LiftIdleParams>(&prev_state)){
    // prev state was idle
    lcd::print(3, "prev_state was idle");
  }
}

// Lift idle state
const char* LiftIdleParams::getName(){
  return "LiftIdle";
}
void LiftIdleParams::handle(){}
void LiftIdleParams::handleStateChange(LIFT_STATE_TYPES_VARIANT prev_state){
  b_lift_m.move_relative(0, 200);
}

// Lift reset state
const char* LiftResetParams::getName(){
  return "LiftReset";
}
void LiftResetParams::handle(){
  b_lift_m.move(-20);
  Timer vel_rise_timer{"vel_rise_timer"};
  while(fabs(b_lift_m.get_actual_velocity()) < 20 && vel_rise_timer.get_time() < 100) _Task_::delay(10);
  while(fabs(b_lift_m.get_actual_velocity()) > 10) _Task_::delay(10);
  b_lift_m.tare_position();
	lift.changeState(LiftIdleParams{});
}
void LiftResetParams::handleStateChange(LIFT_STATE_TYPES_VARIANT prev_state){
  b_lift_m.move_velocity(0);
}

