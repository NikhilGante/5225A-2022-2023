#include "tracking.hpp"
#include "config.hpp"
#include "drive.hpp"
#include "util.hpp"
#include "Libraries/pid.hpp"
#include "Libraries/piston.hpp"
#include "Libraries/timer.hpp"


// Coords of high goal
Vector r_goal{123.0, 18.0}, b_goal{18.0, 123.0};

Tracking tracking; // singleton tracking object
/* Tests:
// 4.4
  8.2
  8.8
  2.7
----
 -3.68
 -3.2
 -6.1
 -5.9
  0.9
*/
static constexpr double TICKS_TO_INCHES_325 = 3.25*std::numbers::pi/36000;
void trackingUpdate(){
  // LeftEncoder.reset(); RightEncoder.reset(); BackEncoder.reset();
  left_tracker.reset_position(); right_tracker.reset_position(); back_tracker.reset_position();
  left_tracker.set_data_rate(5), right_tracker.set_data_rate(5), back_tracker.set_data_rate(5);
  // -1.43
  double dist_lr = 11.32, dist_b = 0.0;  // distance between left and right tracking wheels, and distance from back wheel to tracking centre
  double left, right, back, new_left, new_right, new_back;

  double last_left = left_tracker.get_position()*TICKS_TO_INCHES_325;
  double last_right = -right_tracker.get_position()*TICKS_TO_INCHES_325;
  double last_back = back_tracker.get_position()*TICKS_TO_INCHES_325;

  double theta = 0.0, beta = 0.0, alpha = 0.0;
  double radius_r, radius_b, h_y, h_x;
  double x_x, x_y, y_y, y_x;
  double last_x = 0, last_y = 0, last_a = 0;
  double last_vel_l = 0, last_vel_r = 0, last_vel_b = 0;

  double sin_alpha, cos_alpha, sin_beta; // locals to avoid duplicate computations

  Position last_position; // last position of robot
  Timer velocity_timer{"velocity_timer"};
  Timer tracking_timer{"timer"};

  while(true){


    // if(master.get_digital_new_press(DIGITAL_A)) tracking.reset();
    // else if(master.get_digital_new_press(DIGITAL_UP)) dist_lr += 0.001;
    // else if(master.get_digital_new_press(DIGITAL_DOWN)) dist_lr -= 0.001;
    // lcd::print(3, "dist_lr: %lf", dist_lr);

    new_left = left_tracker.get_position()*TICKS_TO_INCHES_325;
    new_right = -right_tracker.get_position()*TICKS_TO_INCHES_325;
    new_back = back_tracker.get_position()*TICKS_TO_INCHES_325;
    
    lcd::print(2, "l:%lf r:%lf", new_left, new_right);

    // updates how much each side of the robot travelled in inches since the last cycle (left, right and back)
    left = new_left - last_left;
    right = new_right - last_right;
    // back = new_back - last_back;
    back = 0.0;

    if(velocity_timer.getTime() > 50){  // velocity is updated every 20 
      uint32_t velocity_update_time = velocity_timer.getTime(); // time since last velocity update
      tracking.l_vel = (new_left - last_vel_l) / velocity_update_time * 1000; // velocities are in inches per second
      tracking.r_vel = (new_right - last_vel_r) / velocity_update_time * 1000;
      tracking.b_vel = (new_back - last_vel_b) / velocity_update_time * 1000;

      last_vel_l = new_left;
      last_vel_r = new_right;
      last_vel_b = new_back;

      // gives us linear velocity in inches per second, and angular velocity in radians per second
      tracking.g_vel.x = (tracking.g_pos.x - last_position.x) / velocity_update_time * 1000;
      tracking.g_vel.y = (tracking.g_pos.y - last_position.y) / velocity_update_time * 1000;
      tracking.g_vel.a = (tracking.g_pos.a - last_position.a) / velocity_update_time * 1000;

      last_position = tracking.g_pos;

      velocity_timer.reset();
    }
    // update last values
    last_left = new_left;
    last_right = new_right;
    last_back = new_back;

    theta = (left-right)/dist_lr; // change in robot's angle
    if (theta != 0){  // if the robot has travelled in an arc
      radius_r = right/theta;
      radius_b = back/theta;
      beta = theta/2.0;
      sin_beta = sin(beta);
      h_y = (radius_r + dist_lr/2) * 2 * sin_beta; // distance travelled by local y (right wheel) arc
      h_x = (radius_b + dist_b) * 2 * sin_beta; // distance travelled by local x (back wheel) arc
    }
    else{
      h_y = right;
      h_x = back;
      beta = 0.0;
    }
    alpha = tracking.g_pos.a + beta;  // angle of local x and y vectors
    sin_alpha = sin(alpha);
    cos_alpha = cos(alpha);

    x_x = h_x * cos_alpha; // global x movement detected by local x (back wheel) arc
    x_y = h_x * -sin_alpha; // global y movement detected by local x (back wheel) arc
    y_x = h_y * sin_alpha; // global x movement detected by local y (right wheel) arc
    y_y = h_y * cos_alpha; // global y movement detected by local y (right wheel) arc

    tracking.pos_mutex.take();
    tracking.g_pos.x += x_x + y_x;
    tracking.g_pos.y += y_y + x_y;
    tracking.g_pos.a += theta;
    tracking.pos_mutex.give();


    // tracking_log("L:%d R:%d B:%d\n", left_tracker.get_position(), right_tracker.get_position(), back_tracker.get_position());
    if(tracking_timer.getTime() > 50){
      // tracking_log("%lf, %lf, %lf %lf %lf\n", tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a), tracking.g_vel.x, tracking.g_vel.y);
      // tracking_log("%lf, %lf, %lf %lf %lf\n", tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a), tracking.b_vel, (tracking.l_vel + tracking.r_vel)/2);
      // tracking_log("%lf\n", radToDeg(tracking.g_vel.a));

      // tracking_log("x:%lf y:%lf a:%lf\n", tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a));
      tracking_timer.reset();
    }
    // tracking_log("a_vel: %lf\n", radToDeg(tracking.g_vel.a));
    // tracking_log("L:%d R:%d B:%d", LeftEncoder.get_value(), RightEncoder.get_value(), BackEncoder.get_value());
		
    // pros::lcd::print(2, "h_x:%lf, h_y: %lf", h_x, h_y);
    // pros::lcd::print(0, "L:%d R:%d B:%d", new_left, new_right, new_back);
    pros::lcd::print(0, "L:%d R:%d B:%d", left_tracker.get_position(), -right_tracker.get_position(), back_tracker.get_position());
		pros::lcd::print(1, "x:%.2lf y:%.2lf a:%.2lf %.2lf", tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a), fmod(radToDeg(tracking.g_pos.a), 360));

    delay(10);
  }
}

void Tracking::waitForComplete(){
  drive.waitToReachState(DriveIdleParams{});
}

void Tracking::waitForDistance(double distance){
  WAIT_UNTIL(std::abs(drive_error.load()) <= distance);
}

void Tracking::reset(Position pos){
  tracking_log("Resetting tracking from %.2f, %.2f, %.2f to %.2f, %.2f, %.2f\n", tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a), pos.x,pos.y, radToDeg(pos.a));
  tracking.pos_mutex.take();
  tracking.g_pos = pos;
  tracking.pos_mutex.give();
  alert::start(term_colours::NOTIF, "RESET!");
}

void handleBrake(E_Brake_Modes brake_mode){
  switch(brake_mode){
    case E_Brake_Modes::none:
      break;
    case E_Brake_Modes::coast:
      moveDrive(0.0, 0.0);
      break;
    case E_Brake_Modes::brake:
      tracking_log("drivebrake\n");
      driveBrake();
      break;
  }
}

// Wrapper functions for drive states (motion algorithms)
void moveToTargetSync(Vector target, E_Brake_Modes brake_mode, uint8_t max_power, double end_error_x, E_Robot_Sides robot_side){
  drive.changeState(DriveMttParams{target, brake_mode, max_power, end_error_x, robot_side});
  tracking.waitForComplete();
}
void moveToTargetAsync(Vector target, E_Brake_Modes brake_mode, uint8_t max_power, double end_error_x, E_Robot_Sides robot_side){
  drive.changeState(DriveMttParams{target, brake_mode, max_power, end_error_x, robot_side});
}

void turnToAngleSync(double angle, E_Brake_Modes brake_mode, double end_error, double max_power){
  drive.changeState(DriveTurnToAngleParams{angle, brake_mode, end_error, max_power});
  tracking.waitForComplete();
}
void turnToAngleAsync(double angle, E_Brake_Modes brake_mode, double end_error, double max_power){
  drive.changeState(DriveTurnToAngleParams{angle, brake_mode, end_error, max_power});
}

void turnToTargetSync(Vector target, double offset, bool reverse, E_Brake_Modes brake_mode, double end_error, double max_power){
  drive.changeState(DriveTurnToTargetParams{target, offset, reverse, brake_mode, end_error, max_power});
  tracking.waitForComplete();
}

void turnToTargetAsync(Vector target, double offset, bool reverse, E_Brake_Modes brake_mode, double end_error){
  drive.changeState(DriveTurnToTargetParams{target, offset, reverse, brake_mode, end_error});
}

void flattenAgainstWallSync(){
  drive.changeState(DriveFlattenParams{});
  tracking.waitForComplete();
}

void flattenAgainstWallAsync(){
  drive.changeState(DriveFlattenParams{});
}

// Wrapper functions to aim at high goals
void aimAtRed(double offset){
  turnToTargetSync(r_goal, offset);
}
void aimAtBlue(double offset){
  turnToTargetSync(b_goal, offset);
}

void turnToAngleInternal(std::function<double()> getAngleFunc, E_Brake_Modes brake_mode, double end_error, double max_power){
  end_error = degToRad(end_error);
  double angle_kp = max_power > 50 ? 4: 5;

  PID temp(angle_kp, 0.03, 40.0, 0.0, true, 0.0, degToRad(10.0));

  PID angle_pid = temp;

  constexpr double kB = 13.78; // ratio of motor power to target velocity (in radians) i.e. multiply vel by this to get motor power
  Timer motion_timer{"motion_timer"};
  constexpr double kP_vel = 0.0;
  do{
    tracking.drive_error = nearAngle(getAngleFunc(), tracking.g_pos.a);
    double target_velocity = angle_pid.compute(-tracking.drive_error, 0.0);
    double power = kB * target_velocity + kP_vel * (target_velocity - tracking.g_vel.a);
    if(std::abs(power) > max_power) power = sgn(power) * max_power;
    else if(std::abs(power) < tracking.min_move_power_a) power = sgn(power) * tracking.min_move_power_a;
    tracking_log("%d err:%lf power: %lf\n", millis(), radToDeg(angle_pid.getError()), power);
    moveDrive(0.0, power);
    _Task::delay(10);
  }
  while(std::abs(angle_pid.getError()) > end_error); //? why not wait_until
  handleBrake(brake_mode);
  tracking_log("TURN TO ANGLE MOTION DONE took %lld ms | Target:%lf | At x:%lf y:%lf, a:%lf\n", motion_timer.getTime(), radToDeg(tracking.drive_error), tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a));
  drive.changeState(DriveIdleParams{});
}

// STATE MACHINE STUFF 

Machine<DRIVE_STATE_TYPES> drive("Drive", DriveOpControlParams{});

// Drive idle state
void DriveIdleParams::handle(){}
void DriveIdleParams::handleStateChange(driveVariant prev_state){}

// Drive operator control params
void DriveOpControlParams::handle(){
  driveHandleInput();
}
void DriveOpControlParams::handleStateChange(driveVariant prev_state){}


// Drive move to target state
DriveMttParams::DriveMttParams(Vector target, E_Brake_Modes brake_mode, uint8_t max_power, double end_error_x, E_Robot_Sides robot_side) :
 target(target), brake_mode(brake_mode), max_power(max_power), end_error_x(end_error_x), robot_side(robot_side){}

void DriveMttParams::handle(){
  Vector line_error = target - tracking.g_pos;  // Displacement from robot's position to target
  double line_angle = std::numbers::pi/2 - line_error.getAngle();  // Angle of line we're following, relative to the vertical
  line_error.rotate(tracking.g_pos.a);  // Now represents local displacement from robot's position to target
  int8_t power_sgn; // Sign of local y power
  Timer motion_timer{"motion_timer"};
  PID y_pid(5.0, 0.008, 300.0, 0.0, true, 0.0, 8.0);
  // Assigns a sign to power depending on side of robot
  switch(robot_side){
    case E_Robot_Sides::front:
      power_sgn = 1;
      break;
    case E_Robot_Sides::back:
      power_sgn = -1;
      break;
    case E_Robot_Sides::automatic:
      power_sgn = sgn(line_error.getY());
      if(!power_sgn) power_sgn = 1; // Doesn't let power_sgn be 0
      break;
  }
  // tracking_log("power_sgn: %d\n", power_sgn);
  constexpr double kP_a = 2.5;  // proportional multiplier for angular error
  do{
    line_error = target - tracking.g_pos;
    // How much robot has to turn to face target
    // tracking_log("vals %lf %lf\n", M_PI_2 - line_error.getAngle(), (power_sgn == -1 ? std::numbers::pi : 0));
    double error_a = nearAngle((std::numbers::pi/2 - line_error.getAngle()) + (power_sgn == -1 ? std::numbers::pi : 0), tracking.g_pos.a);
    line_error.rotate(line_angle);  // Now represents displacement relative to the line the robot is following
    tracking.drive_error = line_error.getY(); 
    double power_y = y_pid.compute(-power_sgn*line_error.getY(), 0.0);
    if(std::abs(power_y) > max_power) power_y = sgn(power_y) * max_power;
    else if(std::abs(power_y) < tracking.min_move_power_y) power_y = sgn(power_y) * tracking.min_move_power_y;
    
    // Perpendicular distance robot will land from the target travelling at the current orientation
    double error_x = -line_error.getX() + line_error.getY() * tan(nearAngle(tracking.g_pos.a, line_angle));
    // Only corrects if necessary (if robot won't land within an acceptable distance from the target)
    double correction = std::abs(error_x) > end_error_x? kP_a * error_a * sgn(power_sgn): 0.0;
    double left_power, right_power;
    switch(sgn(correction)){
      case 0:
        left_power = power_y, right_power = power_y;
        break;
      case 1:
        left_power = power_y, right_power = power_y * exp(-correction);
        break;
      case -1:
        left_power = power_y * exp(correction), right_power = power_y;
        break;
    }
    // x, y, a, l, r, errA
    // tracking_log("%lf, %lf, %lf, %lf, %lf, %lf\n", tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a), left_power, right_power, radToDeg(error_a));

    // tracking_log("%d l:%lf, r:%lf\n", millis(), left_power, right_power);
    // tracking_log("powers: %lf %lf power_y:%lf error_line_y: %lf\n", left_power, right_power, power_y, line_error.getY());
    // tracking_log("power_y: %lf, error_x: %lf, error_a: %lf\n", power_y, error_x, radToDeg(error_a));

    //tracking_log("%d, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf\n", millis(), tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a), left_power, right_power, power_y, line_error.getY(), error_x, radToDeg(error_a), -line_error.getX(), radToDeg(line_angle), radToDeg(nearAngle(tracking.g_pos.a, line_angle)));

    moveDriveSide(left_power, right_power);
    _Task::delay(10);
  }  
  while(line_error.getY() > 0.5);
  tracking_log("MTT MOTION DONE took %lld ms | Targ x:%lf, y:%lf | At x:%lf y:%lf, a:%lf\n", motion_timer.getTime(), target.getX(), target.getY(), tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a));
  handleBrake(brake_mode);
  drive.changeState(DriveIdleParams{});
}
void DriveMttParams::handleStateChange(driveVariant prev_state){}

// Drive Turn to Angle State
DriveTurnToAngleParams::DriveTurnToAngleParams(double angle, E_Brake_Modes brake_mode, double end_error, double max_power):
  angle(angle), brake_mode(brake_mode), end_error(end_error), max_power(max_power){}

void DriveTurnToAngleParams::handle(){
  turnToAngleInternal(std::function([&](){return degToRad(angle);}), brake_mode, end_error, max_power);
}
void DriveTurnToAngleParams::handleStateChange(driveVariant prev_state){}

// Drive Turn to Target State
DriveTurnToTargetParams::DriveTurnToTargetParams(Vector target, double offset, bool reverse, E_Brake_Modes brake_mode, double end_error, double max_power):
  target(target), offset(offset), reverse(reverse), brake_mode(brake_mode), end_error(end_error), max_power(max_power){}

void DriveTurnToTargetParams::handle(){
  turnToAngleInternal(std::function([&](){
    return std::numbers::pi/2 - (target - tracking.g_pos).getAngle() + degToRad(offset) + (reverse ? std::numbers::pi : 0);
  }), brake_mode, end_error, max_power);
}
void DriveTurnToTargetParams::handleStateChange(driveVariant prev_state){}

// Drive Flatten state

void DriveFlattenParams::handle(){  // Flattens against wall
  Timer motion_timer{"motion_timer"};
  moveDrive(0, 0);
	trans_p.setState(LOW);
  delay(100); // waits for Transmission to shift
  moveDrive(-50, 0);  // moves backwards
  // Waits until velocity rises or takes > 10 cycles (10ms)
  int cycle_count = 0;

  Timer timeout{"timeout"};
  while(timeout.getTime() < 250 && cycle_count < 10){
    tracking_log("FLATTEN 1| l:%lf r:%lf\n", tracking.l_vel, tracking.r_vel);
    if(tracking.l_vel > -3.0 && tracking.r_vel > -3.0)  cycle_count++;
    else cycle_count = 0;
    _Task::delay(10);
  }

  bool l_slow = false, r_slow = false; //
  // Waits until velocity drops (to detect wall)
  cycle_count = 0;
  while(cycle_count < 10){
    tracking_log("FLATTEN 2| l:%lf, r:%lf\n", tracking.l_vel, tracking.r_vel);
    tracking_log("FLATTEN 2| l:%lf, r:%lf\n", tracking.l_vel, tracking.r_vel);
    l_slow = std::abs(tracking.l_vel) < 3.0, r_slow = std::abs(tracking.r_vel) < 3.0;
    if(l_slow){
      if(r_slow){
        moveDrive(-20, 0); // Presses into roller 
        cycle_count++;
      }
      else{
        moveDriveSide(-5, -30); // Turns right
        cycle_count = 0;  // Reset count
      }
    }
    else if(r_slow){
      moveDriveSide(-30, -5); // Turns left
      cycle_count = 0;  // Reset count
    }
    else{
      moveDrive(-50, 0);
      cycle_count = 0;  // reset count
    }
    _Task::delay(10);
  }
  moveDrive(-10, 0); // Applies holding power
  tracking_log("DRIVE FLATTEN DONE, took %lld ms\n", motion_timer.getTime());
  drive.changeState(DriveIdleParams{});
}
void DriveFlattenParams::handleStateChange(driveVariant prev_state){}
