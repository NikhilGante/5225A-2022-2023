#include "intake.hpp"
#include "shooter.hpp"
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
    if(master.get_digital_new_press(intakeToggleBtn) && g_mag_disc_count < 3) intakeOn();
    if(master.get_digital_new_press(intakeRevBtn)) intakeRev();
  }
  else if(get_if<IntakeRevParams>(&cur_state)){
    if(master.get_digital_new_press(intakeToggleBtn) && g_mag_disc_count < 3) intakeOn();
    if(master.get_digital_new_press(intakeRevBtn)) intakeOff();
  }
  /*
  else if(get_if<IntakeRollerParams>(&cur_state)){  // Cancel spinning of roller if roller btn is pressed
    if(master.get_digital_new_press(rollerBtn)){
      // Gives driver back control
      trans_p.setState(HIGH);
      drive.changeState(DriveOpControlParams{});
      intakeOff();
    }
  }
  */
  
  // Spin roller if btn is pressed and not already spinning
  // if(master.get_digital_new_press(rollerBtn) && !get_if<IntakeRollerParams>(&cur_state))  spinRoller(false);

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
  // mag_ds_val = mag_ds.get_value();
  mag_ds_val = mag_disc_thresh + 1;
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
    log("CONTROLLER RUMBLING FROM LINE 72 in file intake.cpp");
    _Task::delay(185);
    intakeOff();
    if(angleOverride)  angler_p.setState(HIGH);
  }

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
  trans_p.setState(HIGH);
  delay(100);
	// FLATTEN CODE
	pros::lcd::print(4, "dist: %lf   ", error);
	moveDrive(-40, 0);
 
	delay(300); // Waits for velocity to rise

  WAIT_UNTIL(tracking.r_vel > -3);

	master.rumble("-");
  log("CONTROLLER RUMBLING FROM LINE 158 in file intake.cpp");

	moveDrive(-10, 0);
  log("Turning roller\n");
  intake_m.move_relative(degrees, 200);  // should be 500 for skills, should be 300 for auton
  WAIT_UNTIL(fabs(intake_m.get_target_position() - intake_m.get_position()) < 10); // wait for intake to reach poisiton 
  log("Finished spinning roller\n");

	master.rumble("-");
  log("CONTROLLER RUMBLING FROM LINE 167 in file intake.cpp");
	moveDrive(0, 0);
  printf("**DONE ROLLER\n");
	roller_timer.print();
  drive.changeState(DriveOpControlParams{});
  master.rumble("-"); // Notifies driver spinning roller has finished
  log("CONTROLLER RUMBLING FROM LINE 173 in file intake.cpp");
	moveDrive(0, 0);
  delay(100);

	trans_p.setState(HIGH);
  intakeOff();

}
void IntakeRollerParams::handleStateChange(INTAKE_STATE_TYPES_VARIANT prev_state){
}

void spinRoller(double degrees){  // Wrapper function to make intake index discs
  intake.changeState(IntakeRollerParams{degrees});
}