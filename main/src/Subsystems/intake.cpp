#include "intake.hpp"
#include "../config.hpp"
#include "../drive.hpp"
#include "../util.hpp"
#include "../tracking.hpp"
#include "../Devices/controller.hpp"
#include "../Devices/motor.hpp"
#include "../Devices/piston.hpp"
#include "shooter.hpp"

Machine<INTAKE_STATE_TYPES> intake{"Intake", IntakeOffParams{}};
std::atomic<int> g_mag_disc_count = 0;

void intakeHandleInput(){
  intakeVariant cur_state = intake.getState();
  if(std::get_if<IntakeOnParams>(&cur_state)){
    if(master.getNewDigital(intakeToggleBtn))  intakeOff();
    if(master.getNewDigital(intakeRevBtn)) intakeRev();
  }
  else if(std::get_if<IntakeOffParams>(&cur_state)){
    if(master.getNewDigital(intakeToggleBtn) && g_mag_disc_count < 3) intakeOn();
    if(master.getNewDigital(intakeRevBtn)) intakeRev();
  }
  else if(std::get_if<IntakeRevParams>(&cur_state)){
    if(master.getNewDigital(intakeToggleBtn) && g_mag_disc_count < 3) intakeOn();
    if(master.getNewDigital(intakeRevBtn)) intakeOff();
  }
}


// Intake idle state
void IntakeIdleParams::handle(){}
void IntakeIdleParams::handleStateChange(intakeVariant prev_state){}

// Intake on state

IntakeOnParams::IntakeOnParams(int8_t speed) : speed(speed){}

void IntakeOnParams::handle(){  // synchronous state
  mag_ds_val = mag_ds.get_value();
  mag_disc_detected = mag_ds_val < mag_disc_thresh;

  if(!mag_disc_detected && mag_disc_detected_last){	// disk just now left mag sensor (entered mag)
    g_mag_disc_count++;
    intake.log("INCR, count: %d", g_mag_disc_count.load());
  }
  mag_disc_detected_last = mag_disc_detected;

  // If mag is full, don't let any more discs in
  // intake.log("%d MAG| %d %d", millis(), mag_ds_val, g_mag_disc_count.load());  
  
  if(g_mag_disc_count >= 3) {
    intake.log("COUNTED 3");
    master.rumble();
    intake.log("CONTROLLER RUMBLING FROM LINE %d in file %s", __LINE__, __FILE__);
    _Task::delay(185);
    intakeOff();
    if(angleOverride)  angler_p.setState(HIGH);
  }

  // intake.log("count:%d", g_mag_disc_count.load());
}
void IntakeOnParams::handleStateChange(intakeVariant prev_state){
  angler_p.setState(LOW);
  intake_m.move(speed);
}

// Wrapper function to turn intake on
void intakeOn(int8_t speed) {intake.changeState(IntakeOnParams{speed});}

// Intake off state
void IntakeOffParams::handle(){}
void IntakeOffParams::handleStateChange(intakeVariant prev_state) {intake_m.move(0);}

void intakeOff() {intake.changeState(IntakeOffParams{});} // Wrapper function to turn intake off

// Intake reverse state

IntakeRevParams::IntakeRevParams(int8_t speed) : speed(speed){}

void IntakeRevParams::handle(){}
void IntakeRevParams::handleStateChange(intakeVariant prev_state) {intake_m.move(speed);}

void intakeRev(int8_t speed) {intake.changeState(IntakeRevParams{speed});} // Wrapper function to turn intake in reverse

// Intake index state

IntakeIndexParams::IntakeIndexParams(int8_t speed) : speed(speed){}

void IntakeIndexParams::handle(){}
void IntakeIndexParams::handleStateChange(intakeVariant prev_state) {intake_m.move(speed);}

void intakeIndex(int8_t speed) {intake.changeState(IntakeIndexParams{speed});} // Wrapper function to make intake index discs


IntakeRollerParams::IntakeRollerParams(double degrees): degrees(degrees){}

void IntakeRollerParams::handle(){
  Timer roller_timer{"Roller", intake.log};
  drive.changeState(DriveIdleParams{});
  drive.waitToReachState(DriveIdleParams{});

  moveDrive(0, 0);
  trans_p.setState(HIGH);
  delay(100);
	// FLATTEN CODE
	intake.log("dist: %lf", error);
	moveDrive(-40, 0);
 
	delay(300); // Waits for velocity to rise

  WAIT_UNTIL(tracking.r_vel > -3);

	master.rumble();
  intake.log("CONTROLLER RUMBLING FROM LINE %d in file %s", __LINE__, __FILE__);

	moveDrive(-10, 0);
  intake.log("Turning roller");
  intake_m.moveRelative(degrees);  // should be 500 for skills, should be 300 for auton
  WAIT_UNTIL(std::abs(intake_m.getTargetPosition() - intake_m.getPosition()) < 10); // wait for intake to reach poisiton 
  intake.log("Finished spinning roller");

	master.rumble();
  intake.log("CONTROLLER RUMBLING FROM LINE %d in file %s", __LINE__, __FILE__);
	moveDrive(0, 0);
  intake.log("**DONE ROLLER\n");
	roller_timer.print();
  drive.changeState(DriveOpControlParams{});
  master.rumble(); // Notifies driver spinning roller has finished
  intake.log("CONTROLLER RUMBLING FROM LINE %d in file %s", __LINE__, __FILE__);
	moveDrive(0, 0);
  delay(100);

	trans_p.setState(HIGH);
  intakeOff();

}
void IntakeRollerParams::handleStateChange(intakeVariant prev_state){}

void spinRoller(double degrees) {intake.changeState(IntakeRollerParams{degrees});} // Wrapper function to make intake index discs