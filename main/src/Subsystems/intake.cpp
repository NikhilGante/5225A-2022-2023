#include "intake.hpp"
#include "../config.hpp"
#include "../Libraries/controller.hpp"
#include "../Libraries/motor.hpp"
#include "../Libraries/piston.hpp"

Machine<INTAKE_STATE_TYPES> intake("Intake", IntakeOffParams{});

void intakeHandleInput(){
  intakeVariant cur_state = intake.getState();
  if(get_if<IntakeOnParams>(&cur_state)){
    if(master.get_digital_new_press(intakeToggleBtn))  intakeOff();
  }
  else if(get_if<IntakeOffParams>(&cur_state)){
    if(master.get_digital_new_press(intakeToggleBtn)) intakeOn();
  }
  else if(get_if<IntakeRevParams>(&cur_state)){
    if(master.get_digital_new_press(resetDiscCountBtn)) g_mag_disc_count = 0;
  }
}

std::atomic<int> g_mag_disc_count = 0;

// Intake idle state
void IntakeIdleParams::handle(){}
void IntakeIdleParams::handleStateChange(intakeVariant prev_state){}

// Intake on state

IntakeOnParams::IntakeOnParams(int8_t speed) : speed(speed){}

void IntakeOnParams::handle(){  // synchronous state
  mag_ds_val = mag_ds.get_value();
  mag_disc_detected = mag_ds_val < mag_disc_thresh;

  // if(!mag_disc_detected && mag_disc_detected_last){	// disk just now left mag sensor (entered mag)
  //   g_mag_disc_count++;
  //   printf("INCR\n");
  // }

  mag_disc_detected_last = mag_disc_detected;

  // end of disc counting code

  // If mag is full, don't let any more discs in
  // if(g_mag_disc_count >= 3) {
  //   delay(100);
  //   intakeRev();
  // }
  // printf("MAG| %d %d %d\n", millis(), mag_ds_val, g_mag_disc_count.load());  
  lcd::print(3, "count:%d", g_mag_disc_count.load());
}
void IntakeOnParams::handleStateChange(intakeVariant prev_state){
  angler_p.setState(LOW);
  intake_m.move(speed);
}

// Wrapper function to turn intake on
void intakeOn(int8_t speed){
  intake.changeState(IntakeOnParams{speed});
}

// Intake off state
void IntakeOffParams::handle(){}
void IntakeOffParams::handleStateChange(intakeVariant prev_state){
  intake_m.move(0);
}

void intakeOff(){  // Wrapper function to turn intake off
  intake.changeState(IntakeOffParams{});
}

// Intake reverse state

IntakeRevParams::IntakeRevParams(int8_t speed) : speed(speed){}

void IntakeRevParams::handle(){
  // If the mag is no longer full, turn intake back on
  if(g_mag_disc_count < 3) intakeOn();
}
void IntakeRevParams::handleStateChange(intakeVariant prev_state){
  intake_m.move(speed);
}

void intakeRev(int8_t speed){  // Wrapper function to turn intake in reverse
  intake.changeState(IntakeRevParams{speed});
}

// Intake index state

IntakeIndexParams::IntakeIndexParams(int8_t speed) : speed(speed){}

void IntakeIndexParams::handle(){}
void IntakeIndexParams::handleStateChange(intakeVariant prev_state){
  intake_m.move(speed);
}

void intakeIndex(int8_t speed){  // Wrapper function to make intake index discs
  intake.changeState(IntakeIndexParams{speed});
}