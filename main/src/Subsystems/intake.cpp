#include "intake.hpp"
#include "pros/misc.hpp"
#include "shooter.hpp"
#include "../tracking.hpp"
#include "../drive.hpp"
#include "../auton.hpp"

Timer intk_off_buzz_timer{"intk_off_buzz_timer"};

Machine<INTAKE_STATE_TYPES> intake("Intake", IntakeOffParams{});

void intakeHandleInput(){
  INTAKE_STATE_TYPES_VARIANT cur_state = intake.getState();
  if(get_if<IntakeOnParams>(&cur_state)){
    if(master.get_digital_new_press(intakeToggleBtn)){
      log("%lld | INTAKE TOGGLE BUTTON PRESSED\n", op_control_timer.getTime());
      intakeOff();
    }  
    if(master.get_digital_new_press(intakeRevBtn)){
      log("%lld | INTAKE REVERSE BUTTON PRESSED\n", op_control_timer.getTime());
      intakeRev();
    } 
  }
  else if(get_if<IntakeOffParams>(&cur_state)){
    if(master.get_digital_new_press(intakeToggleBtn) && g_mag_disc_count < 3){
      log("%lld | INTAKE TOGGLE BUTTON PRESSED\n", op_control_timer.getTime());
      intakeOn();
    }
    if(master.get_digital_new_press(intakeRevBtn)){
      log("%lld | INTAKE REVERSE BUTTON PRESSED\n", op_control_timer.getTime());
      intakeRev();
    } 
  }
  else if(get_if<IntakeRevParams>(&cur_state)){
    if(master.get_digital_new_press(intakeToggleBtn) && g_mag_disc_count < 3){
      log("%lld | INTAKE TOGGLE BUTTON PRESSED\n", op_control_timer.getTime());
      intakeOn();
    }
    if(master.get_digital_new_press(intakeRevBtn)){
      log("%lld | INTAKE REVERSE BUTTON PRESSED\n", op_control_timer.getTime());
      intakeOff();
    } 
  }
 
  // Spin roller if btn is pressed and not already spinning
  if(master.isRising(rollerBtn) && !get_if<IntakeIndexParams>(&cur_state)){
    log("%lld | ROLLER BUTTON RISING\n", op_control_timer.getTime());
    rollerOpControl();
  }

  
  if(get_if<IntakeOffParams>(&cur_state) && intk_off_buzz_timer.getTime() > 300 && g_mag_disc_count < 3){  // Buzzes if in low gear for driver
    intk_off_buzz_timer.reset();
    // master.rumble("..");
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

  /*
  case 1 (3 in mag, no other discs):
    turn off
  case 2 (3 in mag, 1 in uptk):
    turn off, notify driver
    Detection: 3 in mag and line tracker
  case 3 (3 in mag, 1 in intk):
    turn off, notify driver
    Detection: 3 in mag and 1 on lim switch 
  */

  // if (intake_m.get_actual_velocity() < 100) jam_cycle++;
  // else jam_cycle = 0;


  mag_ds_val = uptk_ds.get_value();
  // printf("INTK | %d %d, count: %d\n", millis(), mag_ds_val, g_mag_disc_count.load());
  // mag_ds_val = intk_disc_thresh + 1;
  mag_disc_detected = mag_ds_val < UPTK_DS_THRESH;

  if(!mag_disc_detected && mag_disc_detected_last){	// disk just now left mag sensor (entered mag)
    g_mag_disc_count++;
    printf("INCR, count: %d\n", g_mag_disc_count.load());
    #ifdef LOGS
    printf("INCR\n");
    #endif
  }
  mag_disc_detected_last = mag_disc_detected;

  // If mag is full, don't let any more discs in
  // printf("%d MAG| %d %d\n", millis(), mag_ds_val, g_mag_disc_count.load());  
  
  // if (jam_cycle >= 20){
  //   master.rumble("...");
  //   log("%d INTAKE JAM LMAO\n", millis());

  //   intakeOff();
  // }

  if(g_mag_disc_count >= 3) {
    log("COUNTED 3\n");
    master.rumble("-");
    // log("CONTROLLER RUMBLING FROM LINE 72 in file intake.cpp");


    int x = millis();
    WAIT_UNTIL(millis() - x > 50 || uptk_ds.get_value() < UPTK_DS_THRESH);

    if(!pros::competition::is_autonomous()){
      if(diskInIntake())  intakeOff();
      else intakeRev();

    }
    else intakeOff();
    // _Task::delay(20);

    // Flushes out 4th disc if in auto
    // if(g_mag_disc_count > 3 && pros::competition::is_autonomous()){
    //   drive.changeState(DriveIdleParams{});
    //   drive.waitToReachState(DriveIdleParams{});
    //   intake_m.move(-127);
    //   moveInches(-2.0);
    //   intakeRev();

    // }
    // else 
  }

  printf("Sensors: %d %d %d %d\n", intk_lim_switch.get_value(), intk_dist.get(), intk_ds.get_value(), uptk_ds.get_value());

  // lcd::print(3, "count:%d", g_mag_disc_count.load());
}
void IntakeOnParams::handleStateChange(INTAKE_STATE_TYPES_VARIANT prev_state){
  jam_cycle = 0;
  angler_p.setState(LOW);
  intake_m.move(speed);
  handleRpm();
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

  if(g_mag_disc_count >= 3 && diskInIntake() && possession_notify_timer.getTime() > 200) {
    master.rumble("..");
    possession_notify_timer.reset();
  }
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
void IntakeRevParams::handle(){}
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

  intake_m.move(127);
  WAIT_UNTIL(master.isFalling(rollerBtn));

  intake.changeState(intake.getPrevState());


}
void IntakeRollerParams::handleStateChange(INTAKE_STATE_TYPES_VARIANT prev_state){
}

void rollerOpControl(){  // Wrapper function to make intake index discs
  intake.changeState(IntakeRollerParams{});
}

bool diskInIntake(){
  return intk_ds.get_value() < 2300 || !intk_lim_switch.get_value() || intk_dist.get() < 50 || uptk_ds.get_value() < UPTK_DS_THRESH; 
}