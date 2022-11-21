#include "intake.hpp"

Machine<INTAKE_STATE_TYPES> intake("Intake", IntakeOnParams{});

atomic<int> g_mag_disc_count = 0;

// Intake idle state
const char* IntakeIdleParams::getName(){
  return "IntakeIdle";
}
void IntakeIdleParams::handle(){}
void IntakeIdleParams::handleStateChange(INTAKE_STATE_TYPES_VARIANT prev_state){}

// Intake on state

IntakeOnParams::IntakeOnParams(int8_t speed) : speed(speed){}

const char* IntakeOnParams::getName(){
  return "IntakeOn";
}
void IntakeOnParams::handle(){  // synchronous state
  mag_ds_val = mag_ds.get_value();
  mag_disc_detected = mag_ds_val < mag_disc_thresh;

  if(!mag_disc_detected && mag_disc_detected_last){	// disk just now left mag sensor (entered mag)
    g_mag_disc_count++;
    printf("INCR\n");
  }

  mag_disc_detected_last = mag_disc_detected;

  // end of disc counting code

  // If mag is full, don't let any more discs in
  if(g_mag_disc_count >= 3) intakeRev();
  
  if(master.get_digital_new_press(intakeToggleBtn))  intakeOff();

  printf("%d %d %d\n", millis(), mag_ds_val, g_mag_disc_count.load());  
}
void IntakeOnParams::handleStateChange(INTAKE_STATE_TYPES_VARIANT prev_state){
  intake_m.move(speed);
}

// Wrapper function to turn intake on
void intakeOn(int8_t speed){
  intake.changeState(IntakeOnParams{speed});
}

// Intake off state
const char* IntakeOffParams::getName(){
  return "IntakeOff";
}
void IntakeOffParams::handle(){
  if(master.get_digital_new_press(intakeToggleBtn)) intakeOn();
}
void IntakeOffParams::handleStateChange(INTAKE_STATE_TYPES_VARIANT prev_state){
  intake_m.move(0);
}

void intakeOff(){  // Wrapper function to turn intake off
  intake.changeState(IntakeOffParams{});
}

// Intake reverse state

IntakeRevParams::IntakeRevParams(int8_t speed) : speed(speed){}

const char* IntakeRevParams::getName(){
  return "IntakeRev";
}
void IntakeRevParams::handle(){
  if(master.get_digital_new_press(resetMagBtn)) g_mag_disc_count = 0;
  // If the mag is no longer full, turn intake back on
  if(g_mag_disc_count < 3) intakeOn();
}
void IntakeRevParams::handleStateChange(INTAKE_STATE_TYPES_VARIANT prev_state){
  intake_m.move(speed);
}

void intakeRev(int8_t speed){  // Wrapper function to turn intake in reverse
  intake.changeState(IntakeRevParams{speed});
}

// Intake index state

IntakeIndexParams::IntakeIndexParams(int8_t speed) : speed(speed){}

const char* IntakeIndexParams::getName(){
  return "IntakeIndex";
}
void IntakeIndexParams::handle(){}
void IntakeIndexParams::handleStateChange(INTAKE_STATE_TYPES_VARIANT prev_state){
  intake_m.move(speed);
}

void intakeIndex(int8_t speed){  // Wrapper function to make intake index discs
  intake.changeState(IntakeIndexParams{speed});
}