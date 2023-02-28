#include <cmath>
#include "config.hpp"
#include "tracking.hpp"
#include "Libraries/timer.hpp"
#include "util.hpp"

constexpr double DRIVEBASE_WIDTH = 13.5;
constexpr double LEFT_DIST_OFFSET = 0.5;  // How far in the left sensor is from left edge
constexpr double RIGHT_DIST_OFFSET = 0.5;  // How far in the right sensor is from right edge
constexpr double BACK_DIST_OFFSET = 6.75;  // How far in the ultrasonic is from back edge
constexpr double DISTANCE_DIST_OFFSET = 6.0;  // How far the distance sensor is from the tracking center on the up to down axis


double getDistL(){
  return (l_reset_dist.get()/25.4) - LEFT_DIST_OFFSET + DRIVEBASE_WIDTH/2;
}

double getDistR(){
  return (r_reset_dist.get()/25.4) - RIGHT_DIST_OFFSET + DRIVEBASE_WIDTH/2;
}

double getDistBack(){
  return ((double)(ultra_left.get_value() + ultra_right.get_value())/2/25.4) + BACK_DIST_OFFSET;
}

//x:13.711525 y:10.399731 a:50.518857
Position distanceReset(resetPosition pos, double angleOffset){
  double angle = atan((ultra_left.get_value()-ultra_right.get_value())/(12*25.4));
  printf("angle: %f\n", angle);
  printf("angle: %f\n", degToRad(angle));
  double x, y;
  switch(pos){
    case resetPosition::leftHome:
    printf("cos1: %f, cos2: %f, distBack: %f \n", cos(angle), cos(degToRad(angle)), getDistBack());
      x = (cos(degToRad(angle))*getDistL()) - sin(degToRad(angle))*DISTANCE_DIST_OFFSET;
      y = cos(degToRad(angle))*getDistBack();
      break;
    case resetPosition::rightAway:
      x = 141-cos(degToRad(angle))*getDistBack();
      y = 141-((cos(degToRad(angle))*getDistR()) - sin(degToRad(angle))*DISTANCE_DIST_OFFSET);
      break;
    case resetPosition::leftAway:
      x = 141 - ((cos(degToRad(angle))*getDistL()) - sin(degToRad(angle))*DISTANCE_DIST_OFFSET);
      y = 141-cos(degToRad(angle))*getDistBack();
      break;
    case resetPosition::rightHome:
      x = cos(degToRad(angle))*getDistBack();
      y = (cos(degToRad(angle))*getDistR()) - sin(degToRad(angle))*DISTANCE_DIST_OFFSET;
      break;
  }
  return Position{x, y, angle+degToRad(angleOffset)};
}


// Coords of high goal
Vector r_goal = {123.0, 18.0}, b_goal = {18.0, 123.0};

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
#define TICKS_TO_INCHES 1/36000.0 *(2.75*M_PI);
void trackingUpdate(){
  // LeftEncoder.reset(); RightEncoder.reset(); BackEncoder.reset();
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
  Timer velocity_timer{"velocity_timer"};
  Timer tracking_timer{"timer"};

  double last_gyro_angle = 0.0;

	gyro.tare_rotation();

  while(true){


    // if(master.get_digital_new_press(DIGITAL_A)) tracking.reset();
    // else if(master.get_digital_new_press(DIGITAL_UP)) dist_lr += 0.001;
    // else if(master.get_digital_new_press(DIGITAL_DOWN)) dist_lr -= 0.001;
    // lcd::print(3, "dist_lr: %lf", dist_lr);

    new_left = left_tracker.get_position()*TICKS_TO_INCHES;
    new_right = right_tracker.get_position()*TICKS_TO_INCHES;
    new_back = -back_tracker.get_position()*TICKS_TO_INCHES;
    
    lcd::print(2, "l:%lf r:%lf", new_left, new_right);
    lcd::print(4, "GYRO:%.2lf deg: %.2lf", gyro.get_rotation() * 1.011, radToDeg(tracking.g_pos.a));

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
    
    if(!gyro.is_calibrating()){
      double gyro_angle = gyro.get_rotation() * 1.0027;
      theta = gyro_angle - last_gyro_angle;
      // printf("theta:%.2lf  gyro: %.2lf | %.2lf again:%d \n", theta, gyro_angle, last_gyro_angle, EAGAIN);
      if(fabs(theta) < 0.006) theta = 0.0;  // drift reducer
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


    // printf("L:%d R:%d B:%d\n", left_tracker.get_position(), right_tracker.get_position(), back_tracker.get_position());
    if(tracking_timer.getTime() > 50){
      // log("%lf, %lf, %lf %lf %lf\n", tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a), tracking.g_vel.x, tracking.g_vel.y);
      // log("POS | %lf, %lf, %lf %lf %lf\n", tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a), tracking.b_vel, (tracking.l_vel + tracking.r_vel)/2);
    
      // log("%lf\n", radToDeg(tracking.g_vel.a));

      log("x:%lf y:%lf a:%lf\n", tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a));
      // log("VELS| x:%lf y:%lf a:%lf\n", tracking.g_vel.x, tracking.g_vel.y, radToDeg(tracking.g_vel.a));

      tracking_timer.reset();
    }
    // printf("a_vel: %lf\n", radToDeg(tracking.g_vel.a));
    // printf("L:%d R:%d B:%d", LeftEncoder.get_value(), RightEncoder.get_value(), BackEncoder.get_value());
		
    // pros::lcd::print(2, "h_x:%lf, h_y: %lf", h_x, h_y);
    // pros::lcd::print(0, "L:%d R:%d B:%d", new_left, new_right, new_back);
    pros::lcd::print(0, "L:%d R:%d B:%d", left_tracker.get_position(), right_tracker.get_position(), -back_tracker.get_position());
		pros::lcd::print(1, "x:%.2lf y:%.2lf a:%.2lf %.2lf", tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a), fmod(radToDeg(tracking.g_pos.a), 360));

    delay(10);
  }
}


void Tracking::waitForComplete(){
  drive.waitToReachState(DriveIdleParams{});
}

void Tracking::waitForDistance(double distance){
  WAIT_UNTIL(fabs(drive_error.load()) <= distance);
}

void Tracking::reset(Position pos){
  log("Resetting tracking from %.2f, %.2f, %.2f to %.2f, %.2f, %.2f\n", tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a), pos.x,pos.y, radToDeg(pos.a));
  tracking.pos_mutex.take();
  tracking.g_pos = pos;
  tracking.pos_mutex.give();
  lcd::print(6, "RESET!");

}

void handleBrake(E_Brake_Modes brake_mode){
  switch(brake_mode){
    case E_Brake_Modes::none:
      break;
    case E_Brake_Modes::coast:
      moveDrive(0.0, 0.0);
      break;
    case E_Brake_Modes::brake:
      log("drivebrake\n");
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



void turnToAngleInternal(function<double()> getAngleFunc, E_Brake_Modes brake_mode, double end_error, double max_power){
  end_error = degToRad(end_error);

  PID angle_pid(4.5, 0, 100, 0.0, true, 1.0, degToRad(9.0));

  double kB = 15; // ratio of motor power to target velocity (in radians) i.e. multiply vel by this to get motor power
  Timer motion_timer{"motion_timer"};
  double kP_vel = 5.0;
  do{
    tracking.drive_error = nearAngle(getAngleFunc(), tracking.g_pos.a);
    double target_velocity = angle_pid.compute(-tracking.drive_error, 0.0);
    double power = kB * target_velocity + kP_vel * (target_velocity - tracking.g_vel.a);
    if(fabs(power) > max_power) power = sgn(power) * max_power;
    else if(fabs(power) < tracking.min_move_power_a && fabs(radToDeg(tracking.g_vel.a)) < 30) power = sgn(power) * tracking.min_move_power_a;
    // log("error:%.2lf base:%.2lf p:%.2lf targ_vel:%.2lf vel:%lf power:%.2lf\n", radToDeg(angle_pid.getError()), kB * target_velocity, kP_vel * (target_velocity - tracking.g_vel.a), radToDeg(target_velocity), radToDeg(tracking.g_vel.a), power);
    
    // log("%d err:%lf power: %lf\n", millis(), radToDeg(error), power);
    log("%d, %lf, %lf, %lf, %lf\n", millis(), radToDeg(tracking.drive_error), power, tracking.g_vel.a, radToDeg(target_velocity - tracking.g_vel.a));

    moveDrive(0.0, power);
    _Task::delay(10);
  }
  while(fabs(angle_pid.getError()) > end_error);
  handleBrake(brake_mode);
  log("TURN TO ANGLE MOTION DONE took %lld ms | Target:%lf | At x:%lf y:%lf, a:%lf\n", motion_timer.getTime(), radToDeg(getAngleFunc()), tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a));
  drive.changeState(DriveIdleParams{});
}

// STATE MACHINE STUFF 

Machine<DRIVE_STATE_TYPES> drive("Drive", DriveIdleParams{});

// Drive idle state
const char* DriveIdleParams::getName(){
  return "DriveIdle";
}
void DriveIdleParams::handle(){}
void DriveIdleParams::handleStateChange(DRIVE_STATE_TYPES_VARIANT prev_state){}

// Drive operator control params
const char* DriveOpControlParams::getName(){
  return "DriveOpControl";
}
void DriveOpControlParams::handle(){
  driveHandleInput();
}
void DriveOpControlParams::handleStateChange(DRIVE_STATE_TYPES_VARIANT prev_state){}


// Drive move to target state
DriveMttParams::DriveMttParams(Vector target, E_Brake_Modes brake_mode, uint8_t max_power, double end_error_x, E_Robot_Sides robot_side) :
 target(target), brake_mode(brake_mode), max_power(max_power), end_error_x(end_error_x), robot_side(robot_side){}

const char* DriveMttParams::getName(){
  return "DriveMoveToTarget";
}
void DriveMttParams::handle(){
  Vector line_error = target - tracking.g_pos;  // Displacement from robot's position to target
  double line_angle = M_PI_2 - line_error.getAngle();  // Angle of line we're following, relative to the vertical
  line_error.rotate(tracking.g_pos.a);  // Now represents local displacement from robot's position to target
  int8_t power_sgn; // Sign of local y power
  Timer motion_timer{"motion_timer"};
  PID y_pid(4.5, 0.00, 00.0, 0.0, true, 0.0, 8.0);
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
  // log("power_sgn: %d\n", power_sgn);
  const double kP_a = 2.8;  // proportional multiplier for angular error
  do{
    line_error = target - tracking.g_pos;
    // How much robot has to turn to face target
    // log("vals %lf %lf\n", M_PI_2 - line_error.getAngle(), (power_sgn == -1 ? M_PI : 0));
    double error_a = nearAngle((M_PI_2 - line_error.getAngle()) + (power_sgn == -1 ? M_PI : 0), tracking.g_pos.a);
    line_error.rotate(line_angle);  // Now represents displacement relative to the line the robot is following
    tracking.drive_error = line_error.getY(); 
    double power_y = y_pid.compute(-power_sgn*line_error.getY(), 0.0);
    if(fabs(power_y) > max_power) power_y = sgn(power_y) * max_power;
    else if(fabs(power_y) < tracking.min_move_power_y) power_y = sgn(power_y) * tracking.min_move_power_y;
    
    // Perpendicular distance robot will land from the target travelling at the current orientation
    double error_x = -line_error.getX() + line_error.getY() * tan(nearAngle(tracking.g_pos.a, line_angle));
    // Only corrects if necessary (if robot won't land within an acceptable distance from the target)
    double correction = fabs(error_x) > end_error_x? kP_a * error_a * sgn(power_sgn): 0.0;
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
    // log("%lf, %lf, %lf, %lf, %lf, %lf\n", tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a), left_power, right_power, radToDeg(error_a));

    // log("%d l:%lf, r:%lf\n", millis(), left_power, right_power);
    // log("powers: %lf %lf power_y:%lf error_line_y: %lf\n", left_power, right_power, power_y, line_error.getY());
    // log("power_y: %lf, error_x: %lf, error_a: %lf\n", power_y, error_x, radToDeg(error_a));

    // log("%d, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf\n", millis(), tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a), left_power, right_power, power_y, line_error.getY(), error_x, radToDeg(error_a), -line_error.getX(), radToDeg(line_angle), radToDeg(nearAngle(tracking.g_pos.a, line_angle)));

    moveDriveSide(left_power, right_power);
    _Task::delay(10);
  }  
  while(line_error.getY() > 0.5);
  handleBrake(brake_mode);
  delay(50);  // Waits for brake to be applied
  log("MTT MOTION DONE took %lld ms | Targ x:%lf, y:%lf | At x:%lf y:%lf, a:%lf\n", motion_timer.getTime(), target.getX(), target.getY(), tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a));
  drive.changeState(DriveIdleParams{});
}
void DriveMttParams::handleStateChange(DRIVE_STATE_TYPES_VARIANT prev_state){}

// Drive Turn to Angle State
DriveTurnToAngleParams::DriveTurnToAngleParams(double angle, E_Brake_Modes brake_mode, double end_error, double max_power):
  angle(angle), brake_mode(brake_mode), end_error(end_error), max_power(max_power){}

const char* DriveTurnToAngleParams::getName(){
  return "DriveTurnToAngle";
}
void DriveTurnToAngleParams::handle(){
  turnToAngleInternal(function([&](){return degToRad(angle);}), brake_mode, end_error, max_power);
}
void DriveTurnToAngleParams::handleStateChange(DRIVE_STATE_TYPES_VARIANT prev_state){}

// Drive Turn to Target State
DriveTurnToTargetParams::DriveTurnToTargetParams(Vector target, double offset, bool reverse, E_Brake_Modes brake_mode, double end_error, double max_power):
  target(target), offset(offset), reverse(reverse), brake_mode(brake_mode), end_error(end_error), max_power(max_power){}

const char* DriveTurnToTargetParams::getName(){
  return "DriveTurnToAngle";
}
void DriveTurnToTargetParams::handle(){
  log("Values Before AimAtBlue -- x:%lf y:%lf a:%lf --- Target Angle %lf \n", tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a), radToDeg(M_PI_2 - (b_goal-tracking.g_pos).getAngle()));
  turnToAngleInternal(function([&](){
    return M_PI_2 - (target - tracking.g_pos).getAngle() + degToRad(offset) + (reverse? M_PI : 0);
  }), brake_mode, end_error, max_power);
  log("Values After AimAtBlue -- x:%lf y:%lf a:%lf --- Target Angle %lf \n", tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a), radToDeg(M_PI_2 - (b_goal-tracking.g_pos).getAngle()));

}
void DriveTurnToTargetParams::handleStateChange(DRIVE_STATE_TYPES_VARIANT prev_state){}

// Drive Flatten state

double rpmToInches(double rpm){
  return (rpm / 60) * 3.25 * M_PI * 2/3;
}

const char* DriveFlattenParams::getName(){
  return "DriveFlatten";
}
void DriveFlattenParams::handle(){  // Flattens against wall
  Timer motion_timer{"motion_timer"};
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

		pros::lcd::print(0, "Left: %d   ", ultra_left.get_value());
		pros::lcd::print(1, "Right: %d   ", ultra_right.get_value());
		pros::lcd::print(2, "error: %lf   ", error);

		power = error*0.8;
		if(fabs(error) <= 2) power = 0;
		else if(fabs(power) < 20 && error_rate < 5) power = sgn(error) * 20;
		printf("Err: %lf pow: %lf\n", error, power);

		moveDrive(0, power);
		delay(10);

	}
	while (fabs(error) > 15 && flatten_timeout.getTime() < 1000);
  moveDrive(0, 0); // Applies holding power
	master.rumble("-");
  log("CONTROLLER RUMBLING FROM LINE 458 in file tracking.cpp");
  log("\n\n%d DONE ALIGN\n", millis());


  moveDrive(0, 0); // frees drive
  log("DRIVE ALIGN DONE, took %lld ms\n", motion_timer.getTime());
  drive.changeState(DriveIdleParams{});
}
void DriveFlattenParams::handleStateChange(DRIVE_STATE_TYPES_VARIANT prev_state){}

