#include "tracking.hpp"
#include "config.hpp"
#include "drive.hpp"
#include "Libraries/pid.hpp"
#include "Libraries/timer.hpp"
#include "Devices/controller.hpp"
#include "Devices/piston.hpp"

static Vector r_goal{123.0, 18.0}, b_goal{18.0, 123.0}; //Coords of high goal
Tracking tracking;

double getDistL() {return (l_reset_dist.get()*MM_TO_IN) - LEFT_DIST_OFFSET + HALF_DRIVEBASE_WIDTH;}
double getDistR() {return (r_reset_dist.get()*MM_TO_IN) - RIGHT_DIST_OFFSET + HALF_DRIVEBASE_WIDTH;}
double getDistBack() {return (ultra_left.get_value() + ultra_right.get_value()) * MM_TO_IN / 2.0 + BACK_DIST_OFFSET;}

void trackingUpdate(){
  left_tracker.reset_position(); right_tracker.reset_position(); back_tracker.reset_position();
  left_tracker.set_data_rate(5), right_tracker.set_data_rate(5), back_tracker.set_data_rate(5);
  // -1.43
  double dist_lr = 9.25, dist_b = 6.4;  // distance between left and right tracking wheels, and distance from back wheel to tracking centre
  double left, right, back, new_left, new_right, new_back;

  double last_left = left_tracker.get_position()*TICKS_TO_INCHES;
  double last_right = right_tracker.get_position()*TICKS_TO_INCHES;
  double last_back = -back_tracker.get_position()*TICKS_TO_INCHES;

  double theta = 0.0, beta = 0.0, alpha = 0.0;
  double radius_r, radius_b, h_y, h_x;
  double x_x, x_y, y_y, y_x;
  double last_x = 0, last_y = 0, last_a = 0;
  double last_vel_l = 0, last_vel_r = 0, last_vel_b = 0;

  double sin_alpha, cos_alpha, sin_beta; // locals to avoid duplicate computations

  Position last_position; // last position of robot
  Timer velocity_timer{"Velocity", tracking_log};
  Timer tracking_timer{"Tracking", tracking_log};

  double last_gyro_angle = 0.0;

	gyro.tare_rotation();

  while(true){


    // if(master.getNewDigital(DIGITAL_A)) tracking.reset();
    // else if(master.getNewDigital(DIGITAL_UP)) dist_lr += 0.001;
    // else if(master.getNewDigital(DIGITAL_DOWN)) dist_lr -= 0.001;
    // tracking_log("dist_lr: %lf", dist_lr);

    new_left = left_tracker.get_position()*TICKS_TO_INCHES;
    new_right = right_tracker.get_position()*TICKS_TO_INCHES;
    new_back = -back_tracker.get_position()*TICKS_TO_INCHES;
    
    tracking_log("l:%lf r:%lf\n", new_left, new_right);
    tracking_log("GYRO:%.2lf deg: %.2lf\n", gyro.get_rotation() * 1.011, radToDeg(tracking.getPos().a));

    // updates how much each side of the robot travelled in inches since the last cycle (left, right and back)
    left = new_left - last_left;
    right = new_right - last_right;
    back = new_back - last_back;
    // back = 0.0;

    if(velocity_timer.getTime() > 50){  // velocity is updated every 20 
      uint32_t velocity_update_time = velocity_timer.getTime(); // time since last velocity update
      tracking.l_vel = (new_left - last_vel_l) / velocity_update_time * 1000; // velocities are in inches per second
      tracking.r_vel = (new_right - last_vel_r) / velocity_update_time * 1000;
      tracking.b_vel = (new_back - last_vel_b) / velocity_update_time * 1000;

      last_vel_l = new_left;
      last_vel_r = new_right;
      last_vel_b = new_back;

      // gives us linear velocity in inches per second, and angular velocity in radians per second
      tracking.g_vel.x = (tracking.getPos().x - last_position.x) / velocity_update_time * 1000;
      tracking.g_vel.y = (tracking.getPos().y - last_position.y) / velocity_update_time * 1000;
      tracking.g_vel.a = (tracking.getPos().a - last_position.a) / velocity_update_time * 1000;

      last_position = tracking.getPos();

      velocity_timer.reset();
    }
    // update last values
    last_left = new_left;
    last_right = new_right;
    last_back = new_back;

    theta = (left-right)/dist_lr; // change in robot's angle
    
    if(!gyro.is_calibrating()){
      double gyro_angle = gyro.get_rotation() * 1.0027;
      theta = gyro_angle - last_gyro_angle;
      // printf("theta:%.2lf  gyro: %.2lf | %.2lf again:%d \n", theta, gyro_angle, last_gyro_angle, EAGAIN);
      if(std::abs(theta) < 0.006) theta = 0.0;  // drift reducer
      theta = degToRad(theta);
      last_gyro_angle = gyro_angle;
    }
    else theta = 0.0;


    // 40 inches per second (3 seconds to cross field - 300 cycles): right = 0.4
    // Assumptions - 3 degs travelled (0.0523599 rads), 1 inch radius, 
    // sin(beta) = 0.02617690954
    // radius_R = 0.4/0.0523599 = 7.63943399434
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
    alpha = tracking.getPos().a + beta;  // angle of local x and y vectors
    sin_alpha = sin(alpha);
    cos_alpha = cos(alpha);

    x_x = h_x * cos_alpha; // global x movement detected by local x (back wheel) arc
    x_y = h_x * -sin_alpha; // global y movement detected by local x (back wheel) arc
    y_x = h_y * sin_alpha; // global x movement detected by local y (right wheel) arc
    y_y = h_y * cos_alpha; // global y movement detected by local y (right wheel) arc

    tracking.reset(tracking.getPos() + Position{x_x + y_x, y_y + x_y, theta});


    // tracking_log("L:%d R:%d B:%d\n", left_tracker.get_position(), right_tracker.get_position(), back_tracker.get_position());
    if(tracking_timer.getTime() > 50){
      // tracking_log("%lf, %lf, %lf %lf %lf\n", tracking.getPos().x, tracking.getPos().y, radToDeg(tracking.getPos().a), tracking.g_vel.x, tracking.g_vel.y);
      // tracking_log("POS | %lf, %lf, %lf %lf %lf\n", tracking.getPos().x, tracking.getPos().y, radToDeg(tracking.getPos().a), tracking.b_vel, (tracking.l_vel + tracking.r_vel)/2);
      // tracking_log("%lf\n", radToDeg(tracking.g_vel.a));

      tracking_log("x:%lf y:%lf a:%lf\n", tracking.getPos().x, tracking.getPos().y, radToDeg(tracking.getPos().a));
      // tracking_log("VEL| x:%lf y:%lf a:%lf\n", tracking.g_vel.x, tracking.g_vel.y, radToDeg(tracking.g_vel.a));

      tracking_timer.reset();
    }
    // tracking_log("a_vel: %lf\n", radToDeg(tracking.g_vel.a));
    // tracking_log("L:%d R:%d B:%d", LeftEncoder.get_value(), RightEncoder.get_value(), BackEncoder.get_value());
		
    // tracking_log("h_x:%lf, h_y: %lf", h_x, h_y);
    // tracking_log("L:%d R:%d B:%d", new_left, new_right, new_back);
    tracking_log("L:%d R:%d B:%d\n", left_tracker.get_position(), right_tracker.get_position(), -back_tracker.get_position());
		tracking_log("x:%.2lf y:%.2lf a:%.2lf %.2lf", tracking.getPos().x, tracking.getPos().y, radToDeg(tracking.getPos().a), fmod(radToDeg(tracking.getPos().a), 360));

    _Task::delay(10);
  }
}

void Tracking::waitForComplete() {drive.waitToReachState(DriveIdleParams{});}
void Tracking::waitForDistance(double distance) {WAIT_UNTIL(std::abs(drive_error.load()) <= distance);}

void Tracking::init(Position pos){
  reset(pos);
  task.start(trackingUpdate);
}

void Tracking::reset(Position pos){
  pos_mutex.take();
  // tracking_log("Resetting tracking from %.2f, %.2f, %.2f to %.2f, %.2f, %.2f\n", g_pos.x, g_pos.y, radToDeg(g_pos.a), pos.x,pos.y, radToDeg(pos.a));
  g_pos = pos;
  pos_mutex.give();
  // alert::start(term_colours::NOTIF, "RESET!");
}

Position Tracking::getPos(){
  // pos_mutex.take();
  // Position pos = g_pos;
  // pos_mutex.give();
  // return pos;

  return g_pos;
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
void aimAtRed(double offset, double max_power, double end_error){
  turnToTargetSync(r_goal, offset, false, E_Brake_Modes::brake, end_error, max_power);
}
void aimAtBlue(double offset, double max_power, double end_error){
  turnToTargetSync(b_goal, offset, false, E_Brake_Modes::brake, end_error, max_power);
}

// power 400
// 127 400
// 100 350

void turnToAngleInternal(std::function<double()> getAngleFunc, E_Brake_Modes brake_mode, double end_error, double max_power){
  end_error = degToRad(end_error);

  PID angle_pid("Turn to Angle", tracking_log, 4.5, 0, 100, 0.0, true, 1.0, degToRad(9.0));

  constexpr double kB = 15; // ratio of motor power to target velocity (in radians) i.e. multiply vel by this to get motor power
  Timer motion_timer{"Motion", tracking_log};
  constexpr double kP_vel = 5;
  do{
    tracking.drive_error = nearAngle(getAngleFunc(), tracking.getPos().a);
    double target_velocity = angle_pid.compute(-tracking.drive_error, 0.0);
    double power = kB * target_velocity + kP_vel * (target_velocity - tracking.g_vel.a);
    if(std::abs(power) > max_power) power = sgn(power) * max_power;
    else if(std::abs(power) < tracking.min_move_power_a && std::abs(radToDeg(tracking.g_vel.a)) < 30) power = sgn(power) * tracking.min_move_power_a;
    // tracking_log("error:%.2lf base:%.2lf p:%.2lf targ_vel:%.2lf vel:%lf power:%.2lf\n", radToDeg(angle_pid.getError()), kB * target_velocity, kP_vel * (target_velocity - tracking.g_vel.a), radToDeg(target_velocity), radToDeg(tracking.g_vel.a), power);
    
    // tracking_log("%d err:%lf power: %lf\n", millis(), radToDeg(error), power);
    tracking_log("%d, %lf, %lf, %lf\n", millis(), radToDeg(tracking.drive_error), power, radToDeg(target_velocity - tracking.g_vel.a));

    moveDrive(0.0, power);
    _Task::delay(10);
  }
  while(std::abs(angle_pid.getError()) > end_error);
  handleBrake(brake_mode);
  tracking_log("TURN TO ANGLE MOTION DONE took %lld ms | Target:%lf | At x:%lf y:%lf, a:%lf\n", motion_timer.getTime(), radToDeg(tracking.drive_error), tracking.getPos().x, tracking.getPos().y, radToDeg(tracking.getPos().a));
  drive.changeState(DriveIdleParams{});
}

// STATE MACHINE STUFF 

Machine<DRIVE_STATE_TYPES> drive("Drive", DriveIdleParams{});

// Drive idle state
void DriveIdleParams::handle(){}
void DriveIdleParams::handleStateChange(driveVariant prev_state){}

// Drive operator control params
void DriveOpControlParams::handle() {driveHandleInput();}
void DriveOpControlParams::handleStateChange(driveVariant prev_state){}


// Drive move to target state
DriveMttParams::DriveMttParams(Vector target, E_Brake_Modes brake_mode, uint8_t max_power, double end_error_x, E_Robot_Sides robot_side) :
target(target), brake_mode(brake_mode), max_power(max_power), end_error_x(end_error_x), robot_side(robot_side){}

void DriveMttParams::handle(){
  Vector line_error = target - tracking.getPos();  // Displacement from robot's position to target
  double line_angle = std::numbers::pi/2 - line_error.getAngle();  // Angle of line we're following, relative to the vertical
  line_error.rotate(tracking.getPos().a);  // Now represents local displacement from robot's position to target
  int8_t power_sgn; // Sign of local y power
  Timer motion_timer{"motion_timer", tracking_log};
  PID y_pid("Move to Target", tracking_log, 4.5, 0, 0, 0, true, 0, 8);
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
  constexpr double kP_a = 2.8;  // proportional multiplier for angular error
  do{
    line_error = target - tracking.getPos();
    // How much robot has to turn to face target
    // tracking_log("vals %lf %lf\n", M_PI_2 - line_error.getAngle(), (power_sgn == -1 ? std::numbers::pi : 0));
    double error_a = nearAngle((std::numbers::pi/2 - line_error.getAngle()) + (power_sgn == -1 ? std::numbers::pi : 0), tracking.getPos().a);
    line_error.rotate(line_angle);  // Now represents displacement relative to the line the robot is following
    tracking.drive_error = line_error.getY(); 
    double power_y = y_pid.compute(-power_sgn*line_error.getY(), 0.0);
    if(std::abs(power_y) > max_power) power_y = sgn(power_y) * max_power;
    else if(std::abs(power_y) < tracking.min_move_power_y) power_y = sgn(power_y) * tracking.min_move_power_y;
    
    // Perpendicular distance robot will land from the target travelling at the current orientation
    double error_x = -line_error.getX() + line_error.getY() * tan(nearAngle(tracking.getPos().a, line_angle));
    // Only corrects if necessary (if robot won't land within an acceptable distance from the target)
    double correction = std::abs(error_x) > end_error_x ? kP_a * error_a * sgn(power_sgn): 0.0;
    double left_power, right_power;
    switch(sgn(correction)){
      case 0:
        left_power = power_y, right_power = power_y;
        break;
      case 1:
        left_power = power_y, right_power = power_y * std::exp(-correction);
        break;
      case -1:
        left_power = power_y * std::exp(correction), right_power = power_y;
        break;
    }
    // x, y, a, l, r, errA
    // tracking_log("%lf, %lf, %lf, %lf, %lf, %lf\n", tracking.getPos().x, tracking.getPos().y, radToDeg(tracking.getPos().a), left_power, right_power, radToDeg(error_a));

    // tracking_log("%d l:%lf, r:%lf\n", millis(), left_power, right_power);
    // tracking_log("powers: %lf %lf power_y:%lf error_line_y: %lf\n", left_power, right_power, power_y, line_error.getY());
    // tracking_log("power_y: %lf, error_x: %lf, error_a: %lf\n", power_y, error_x, radToDeg(error_a));

    //tracking_log("%d, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf\n", millis(), tracking.getPos().x, tracking.getPos().y, radToDeg(tracking.getPos().a), left_power, right_power, power_y, line_error.getY(), error_x, radToDeg(error_a), -line_error.getX(), radToDeg(line_angle), radToDeg(nearAngle(tracking.getPos().a, line_angle)));

    moveDriveSide(left_power, right_power);
    _Task::delay(10);
  }  
  while(line_error.getY() > 0.5);
  handleBrake(brake_mode);
  delay(50);  // Waits for brake to be applied
  tracking_log("MTT MOTION DONE took %lld ms | Targ x:%lf, y:%lf | At x:%lf y:%lf, a:%lf\n", motion_timer.getTime(), target.getX(), target.getY(), tracking.getPos().x, tracking.getPos().y, radToDeg(tracking.getPos().a));
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
  drive.log("Values Before AimAtBlue -- x:%lf y:%lf a:%lf --- Target Angle %lf \n", tracking.getPos().x, tracking.getPos().y, radToDeg(tracking.getPos().a), radToDeg(std::numbers::pi/2 - (b_goal-tracking.getPos()).getAngle()));
  turnToAngleInternal(std::function([&](){
    return std::numbers::pi/2 - (target - tracking.getPos()).getAngle() + degToRad(offset) + (reverse ? std::numbers::pi : 0);
  }), brake_mode, end_error, max_power);
  drive.log("Values After AimAtBlue -- x:%lf y:%lf a:%lf --- Target Angle %lf \n", tracking.getPos().x, tracking.getPos().y, radToDeg(tracking.getPos().a), radToDeg(std::numbers::pi/2 - (b_goal-tracking.getPos()).getAngle()));

}
void DriveTurnToTargetParams::handleStateChange(driveVariant prev_state){}

// Drive Flatten state

double rpmToInches(double rpm){
  return (rpm / 60) * 3.25 * M_PI * 2/3;
}
void DriveFlattenParams::handle(){  // Flattens against wall
  Timer motion_timer{"Drive Flatten", tracking_log};
  double error;
	trans_p.setState(LOW);
	delay(100); // wait for tranmission to shift
	Timer flatten_timeout{"flatten_timeout"};
	double power;
	double error_rate, last_error;
	do {
		error  = ultra_right.get_value()-ultra_left.get_value();
		error_rate = error - last_error;
		last_error = error;

		tracking_log("Left: %d ", ultra_left.get_value());
		tracking_log("Right: %d ", ultra_right.get_value());
		tracking_log("error: %lf\n", error);

		power = error*0.8;
		if(std::abs(error) <= 2) power = 0;
		else if(std::abs(power) < 20 && error_rate < 5) power = sgn(error) * 20;
		printf2("Err: %lf pow: %lf\n", error, power);

		moveDrive(0, power);
		delay(10);

	}
	while (std::abs(error) > 15 && flatten_timeout.getTime() < 1000);
  moveDrive(0, 0); // Applies holding power
	master.rumble();
  tracking_log("CONTROLLER RUMBLING FROM LINE 458 in file tracking.cpp");
  tracking_log("\n\n%d DONE ALIGN\n", millis());


  moveDrive(0, 0); // frees drive
  tracking_log("DRIVE ALIGN DONE, took %lld ms\n", motion_timer.getTime());
  drive.changeState(DriveIdleParams{});
}
void DriveFlattenParams::handleStateChange(driveVariant prev_state){}
