#include "intake.hpp"
#include "../tracking.hpp"
#include "../drive.hpp"

Machine<INTAKE_STATE_TYPES> intake("Intake", IntakeOffParams{});

void intakeHandleInput(){
  INTAKE_STATE_TYPES_VARIANT cur_state = intake.getState();
  if(get_if<IntakeOnParams>(&cur_state)){
    if(master.get_digital_new_press(intakeToggleBtn))  intakeOff();
    if(master.get_digital_new_press(intakeRevBtn)) intakeRev();
  }
  else if(get_if<IntakeOffParams>(&cur_state)){
    if(master.get_digital_new_press(intakeToggleBtn)) intakeOn();
    if(master.get_digital_new_press(intakeRevBtn)) intakeRev();
  }
  else if(get_if<IntakeRevParams>(&cur_state)){
    if(master.get_digital_new_press(intakeToggleBtn)) intakeOn();
  }
}

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
    #ifdef LOGS
    printf("INCR\n");
    #endif
  }

  mag_disc_detected_last = mag_disc_detected;

  // end of disc counting code

  // If mag is full, don't let any more discs in
  
  if(g_mag_disc_count >= 3) {
    master.rumble("-");
    delay(75);
    intakeOff();
    // delay(200);
    // angler_p.setState(HIGH);
  }

  // #ifdef LOGS printf("MAG| %d %d %d\n", millis(), mag_ds_val, g_mag_disc_count.load());  
  // lcd::print(3, "count:%d", g_mag_disc_count.load());
}
void IntakeOnParams::handleStateChange(INTAKE_STATE_TYPES_VARIANT prev_state){
  angler_p.setState(LOW);
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
void IntakeOffParams::handle(){}
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


IntakeRollerParams::IntakeRollerParams(){}

const char* IntakeRollerParams::getName(){
  return "IntakeRoller";
}
void IntakeRollerParams::handle(){
  roller_sensor.set_led_pwm(100);
  flattenAgainstWallSync();
	// delay(200);	// Waits for LED to turn on and robot to touch roller
	intake_m.move(-127);
	Timer roller_timer{"roller_timer"};
  // Switches to opposite colour it saw
  const int thresh = 3000;
  double init_value = roller_sensor.get_rgb().red;
  printf("init_value: %lf\n", init_value);
  // waits
  double cur_val;
  do{
		roller_sensor.set_led_pwm(100);
    cur_val = roller_sensor.get_rgb().red;
    printf("r: %lf \n", cur_val);
    delay(10);
  }while(fabs(cur_val - init_value) < 500);
	roller_timer.print();
	intake_m.move(0);
  intake.changeState(IntakeOffParams{});
}
void IntakeRollerParams::handleStateChange(INTAKE_STATE_TYPES_VARIANT prev_state){
  intake_m.move(-127);
	roller_sensor.set_led_pwm(100);
}

void spinRoller(){  // Wrapper function to make intake index discs
  intake.changeState(IntakeRollerParams{});
}