#include "intake.hpp"
#include "../config.hpp"
#include "../drive.hpp"
#include "../Libraries/timer.hpp"
#include "../tracking.hpp"
#include "../Devices/controller.hpp"
#include "../Devices/motor.hpp"
#include "../Devices/piston.hpp"

Machine<INTAKE_STATE_TYPES> intake("Intake", IntakeOffParams{});

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

std::atomic<int> g_mag_disc_count = 0;

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


IntakeRollerParams::IntakeRollerParams(bool flatten): flatten(flatten){}

void IntakeRollerParams::handle(){
  Timer led{"timer", intake.log};
  roller_sensor.set_led_pwm(100);
  device_log("%d | flatten:%d\n", millis(), flatten);

  if(flatten) flattenAgainstWallSync();
  trans_p.setState(LOW);
  WAIT_UNTIL(led.getTime() > 200 && roller_sensor.get_rgb().red != 0);  // Waits for LED to turn on and robot to touch roller
	intake_m.move(-127);
	Timer roller_timer{"roller_timer", intake.log};
  // Switches to opposite colour it saw

  intake_m.moveRelative(-650);  //should be 450, For skills, should be 650
  WAIT_UNTIL(std::abs(intake_m.getTargetPosition() - intake_m.getPosition()) < 10); // wait for intake to reach poisiton 
  /*
  const int thresh = 3000;
  double init_value = roller_sensor.get_rgb().red;
  device_log("roller init_value: %lf, %lf\n", init_value, roller_sensor.get_rgb().blue);
  // waits to see a value > 700  different than inital value (waits for a colour change)
  double cur_val;
  Timer timeout{"timeout", intake.log};
  do{
		roller_sensor.set_led_pwm(100);
    cur_val = roller_sensor.get_rgb().red;
    // device_log("r: %lf \n", cur_val);
    device_log("%d, %lf, %lf \n", millis(), roller_sensor.get_rgb().red, roller_sensor.get_rgb().blue);
    _Task::delay(100);
  } while(cur_val < init_value*2 && cur_val > init_value / 2  && timeout.getTime() < 1500);
  */
  intake.log("**DONE ROLLER\n");
  // while(std::abs(cur_val - init_value) < 800 && timeout.getTime() < 1500);
	roller_timer.print();
  drive.changeState(DriveOpControlParams{});
  master.rumble(); // Notifies driver spinning roller has finished
	moveDrive(0, 0);
  delay(100);

	trans_p.setState(HIGH);
  intakeOff();

}
void IntakeRollerParams::handleStateChange(intakeVariant prev_state){}

void spinRoller(bool flatten) {intake.changeState(IntakeRollerParams{flatten});} // Wrapper function to make intake index discs