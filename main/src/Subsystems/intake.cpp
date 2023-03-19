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
    intake.changeState(IntakeIdleParams{});
    intake.waitToReachState(IntakeIdleParams{});
    intake_m.move(127); // Operates intake manually so disc count doesn't turn it off
  }
  else if (master.isFalling(rollerBtn)){
    log("%lld | ROLLER BUTTON RELEASED\n", op_control_timer.getTime());
    intakeOff();
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
  mag_ds_val = intk_ds.get_value();
  // printf("INTK | %d %d, count: %d\n", millis(), mag_ds_val, g_mag_disc_count.load());
  // mag_ds_val = mag_disc_thresh + 1;
  mag_disc_detected = mag_ds_val < mag_disc_thresh;

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
  
  if(g_mag_disc_count >= 3) {
    log("COUNTED 3\n");
    master.rumble("-");
    // log("CONTROLLER RUMBLING FROM LINE 72 in file intake.cpp");
    _Task::delay(140);

    // Flushes out 4th disc if in auto
    // if(g_mag_disc_count > 3 && pros::competition::is_autonomous()){
    //   drive.changeState(DriveIdleParams{});
    //   drive.waitToReachState(DriveIdleParams{});
    //   intake_m.move(-127);
    //   moveInches(-2.0);
    //   intakeRev();

    // }
    // else 
    intakeOff();
    if(angleOverride)  angler_p.setState(HIGH);
  }

  // lcd::print(3, "count:%d", g_mag_disc_count.load());
}
void IntakeOnParams::handleStateChange(INTAKE_STATE_TYPES_VARIANT prev_state){
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


IntakeRollerParams::IntakeRollerParams(double degrees): degrees(degrees){}

const char* IntakeRollerParams::getName(){
  return "IntakeRoller";
}
void IntakeRollerParams::handle(){
  Timer roller_timer{"roller_timer"};
  drive.changeState(DriveIdleParams{});
  drive.waitToReachState(DriveIdleParams{});

  moveDrive(0, 0);
	// FLATTEN CODE
	pros::lcd::print(4, "dist: %lf   ", error);
	moveDrive(-40, 0);
 
	delay(300); // Waits for velocity to rise

  WAIT_UNTIL(tracking.r_vel > -3);

	master.rumble("-");
  // log("CONTROLLER RUMBLING FROM LINE 158 in file intake.cpp");

	moveDrive(-10, 0);
  log("Turning roller\n");
  intake_m.move_relative(degrees, 200);  // should be 500 for skills, should be 300 for auton
  WAIT_UNTIL(fabs(intake_m.get_target_position() - intake_m.get_position()) < 10); // wait for intake to reach poisiton 
  log("Finished spinning roller\n");

	master.rumble("-");
  // log("CONTROLLER RUMBLING FROM LINE 167 in file intake.cpp");
	moveDrive(0, 0);
  printf("**DONE ROLLER\n");
	roller_timer.print();
  drive.changeState(DriveOpControlParams{});
  master.rumble("-"); // Notifies driver spinning roller has finished
  // log("CONTROLLER RUMBLING FROM LINE 173 in file intake.cpp");
	moveDrive(0, 0);
  intakeOff();

}
void IntakeRollerParams::handleStateChange(INTAKE_STATE_TYPES_VARIANT prev_state){
}

void spinRoller(double degrees){  // Wrapper function to make intake index discs
  intake.changeState(IntakeRollerParams{degrees});
}