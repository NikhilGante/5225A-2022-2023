#include <cmath>
#include "config.hpp"
#include "tracking.hpp"
#include "Libraries/pid.hpp"
#include "Libraries/timer.hpp"
#include "util.hpp"
#include "drive.hpp"

Tracking tracking; // singleton tracking object

#define TICKS_TO_INCHES 1/36000.0 *(2.75*M_PI);
void trackingUpdate(){
  // LeftEncoder.reset(); RightEncoder.reset(); BackEncoder.reset();
  left_tracker.reset_position(); right_tracker.reset_position(); back_tracker.reset_position();
  left_tracker.set_data_rate(5), right_tracker.set_data_rate(5), back_tracker.set_data_rate(5);
  double dist_lr = 9.64, dist_b = 10.0;  // distance between left and right tracking wheels, and distance from back wheel to tracking centre
  double left, right, back, new_left, new_right, new_back;

  double last_left = left_tracker.get_position()*TICKS_TO_INCHES;
  double last_right = -right_tracker.get_position()*TICKS_TO_INCHES;
  double last_back = back_tracker.get_position()*TICKS_TO_INCHES;
  
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
    new_left = left_tracker.get_position()*TICKS_TO_INCHES;
    new_right = -right_tracker.get_position()*TICKS_TO_INCHES;
    new_back = back_tracker.get_position()*TICKS_TO_INCHES;

    // updates how much each side of the robot travelled in inches since the le(left, right and back)
    left = new_left - last_left;
    right = new_right - last_right;
    back = new_back - last_back;

    if(velocity_timer.get_time() > 50){  // velocity is updated every 20 
      uint32_t velocity_update_time = velocity_timer.get_time(); // time since last velocity update
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

    tracking.g_pos.x += x_x + y_x;
    tracking.g_pos.y += y_y + x_y;
    tracking.g_pos.a += theta;

    // printf("L:%d R:%d B:%d\n", left_tracker.get_position(), right_tracker.get_position(), back_tracker.get_position());
    // if(tracking_timer.get_time() > 50){
    //   // log("%lf, %lf, %lf\n", tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a));
    //   log("%lf\n", radToDeg(tracking.g_vel.a));

    //   // log("x:%lf y:%lf a:%lf\n", tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a));
    //   tracking_timer.reset();
    // }
    // printf("L:%d R:%d B:%d", LeftEncoder.get_value(), RightEncoder.get_value(), BackEncoder.get_value());
		pros::lcd::print(0, "L:%d R:%d B:%d", left_tracker.get_position(), right_tracker.get_position(), back_tracker.get_position());
		pros::lcd::print(1, "x:%lf y:%lf a:%lf", tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a));

    delay(10);
  }
}

void Tracking::waitForComplete(){
  // WAIT_UNTIL()  // state and next == idle
}

void Tracking::waitForDistance(double distance){
  WAIT_UNTIL(drive_error <= distance);
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

void moveToTarget(Vector target, E_Brake_Modes brake_mode, uint8_t max_power, double end_error_x, E_Robot_Sides robot_side){
  Vector line_error = target - tracking.g_pos;  // Displacement from robot's position to target
  double line_angle = M_PI_2 - line_error.getAngle();  // Angle of line we're following, relative to the vertical
  line_error.rotate(tracking.g_pos.a);  // Now represents local displacement from robot's position to target
  int8_t power_sgn; // Sign of local y power
  Timer motion_timer{"motion_timer"};
  PID y_pid(5.0, 0.001, 0.0, 0.0);
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
  log("power_sgn: %d\n", power_sgn);
  const double kP_a = 6.0;  // proportional multiplier for angular error
  do{
    line_error = target - tracking.g_pos;
    // How much robot has to turn to face target
    log("vals %lf %lf\n", M_PI_2 - line_error.getAngle(), (power_sgn == -1 ? M_PI : 0));
    double error_a = nearAngle((M_PI_2 - line_error.getAngle()) + (power_sgn == -1 ? M_PI : 0), tracking.g_pos.a);
    line_error.rotate(line_angle);  // Now represents displacement relative to the line the robot is following
    double power_y = y_pid.compute(-power_sgn*line_error.getY(), 0.0);  // RUN PID HERE
    if(fabs(power_y) < tracking.min_move_power_y) power_y = sgn(power_y) * tracking.min_move_power_y;
    // Perpendicular distance robot will land from the target travelling at the current orientation
    double error_x = line_error.getX() + line_error.getY() * tan(nearAngle(tracking.g_pos.a, line_angle));
    // Only corrects if necessary (if robot won't land within an acceptable distance from the target)
    double correction = fabs(error_x) > end_error_x? kP_a * error_a * sgn(power_sgn): 0.0;
    // log("power_y: %lf, error_x: %lf, error_a: %lf\n", power_y, error_x, radToDeg(error_a));
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
    log("%lf, %lf, %lf, %lf, %lf, %lf\n", tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a), left_power, right_power, radToDeg(error_a));

    // log("%d l:%lf, r:%lf\n", millis(), left_power, right_power);
    moveDriveSide(left_power, right_power);
    delay(10);
  }
  while(line_error.getY() > 0.5);
  log("MOTION DONE %lld\n", motion_timer.get_time());
  handleBrake(brake_mode);
}

void turnToAngle(double angle, E_Brake_Modes brake_mode, double end_error){
  end_error = degToRad(end_error);
  angle = degToRad(angle);
  PID angle_pid(5.0, 0.02, 40.0, 0.0, true, 0.0, degToRad(10.0));
  PID velocity_pid(0.0, 0.0, 0.0, 0.0);
  double kB = 18.2; // ratio of motor power to target velocity (in radians) i.e. multiply vel by this to get motor power
  Timer motion_timer{"motion_timer"};
  double power;
  double kP_vel = 15.0;
  do{
    double target_velocity = angle_pid.compute(-nearAngle(angle, tracking.g_pos.a), 0.0);
    power = kB * target_velocity + kP_vel * (target_velocity - tracking.g_vel.a);
    if(fabs(power) > 127) power = sgn(power) * 127;
    log("error:%.2lf base:%.2lf p:%.2lf targ_vel:%.2lf vel:%lf power:%.2lf\n", radToDeg(angle_pid.getError()), kB * target_velocity, kP_vel * (target_velocity - tracking.g_vel.a), radToDeg(target_velocity), radToDeg(tracking.g_vel.a), power);
    moveDrive(0.0, power);
    delay(10);
  }
  while(fabs(angle_pid.getError()) > end_error);
  handleBrake(brake_mode);
  log("MOTION DONE %lld\n", motion_timer.get_time());
  lcd::print(7, "MOTION DONE %lld\n", motion_timer.get_time());
}

// 3.606818, -11.877213, -62.104642, -66.909224, -0.000218, 120.628709
// vals 4.183958 3.141593


// this one
// vals 3.605240 3.141593
// 0.000000, 0.000000, 0.000000, -111.803399, -6.923049, 26.565051