#include <cmath>
#include "config.hpp"
#include "tracking.hpp"
#include "Libraries/pid.hpp"
#include "Libraries/timer.hpp"
#include "util.hpp"
#include "drive.hpp"

Tracking tracking; // singleton tracking object

#define TICKS_TO_INCHES (1/36000.0 *(2.75*M_PI));
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

  while(true){
    new_left = left_tracker.get_position()*TICKS_TO_INCHES;
    new_right = -right_tracker.get_position()*TICKS_TO_INCHES;
    new_back = back_tracker.get_position()*TICKS_TO_INCHES;

    // updates how much each side of the robot travelled in inches since the le(left, right and back)
    left = new_left - last_left;
    right = new_right - last_right;
    back = new_back - last_back;

    if(velocity_timer.get_time() > 20){  // velocity is updated every 20 
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
    // printf("x:%lf y:%lf a:%lf\n", tracking.g_pos.x, tracking.g_pos.y, radToDeg(tracking.g_pos.a));
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

void moveToTarget(Vector target, E_Brake_Modes brake_mode = E_Brake_Modes::brake, uint8_t max_power = 127, double end_error_x = 0.5, double decel_start = 0.0, uint8_t exit_power = 0, E_Robot_Sides robot_side = E_Robot_Sides::automatic){
  Vector line_error = target - tracking.g_pos;  // Displacement from robot's position to target
  double line_angle = M_PI_2 - line_error.getAngle();  // Angle of line we're following, relative to the vertical
  line_error.rotate(tracking.g_pos.a);  // Now represents local displacement from robot's position to target
  int8_t power_sgn; // Sign of local y power
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
  const double kP_a = 1.0;  // proportional multiplier for angular error ~8.0
  do{
    line_error = target - tracking.g_pos;
    // How much robot has to turn to face target
    double error_a = nearAngle((M_PI_2 - line_error.getAngle()) + (power_sgn == -1 ? M_PI : 0), tracking.g_pos.a);
    line_error.rotate(line_angle);  // Now represents displacement relative to the line the robot is following
    double power_y = line_error.getY();  // RUN PID HERE
    // Perpendicular distance robot will land from the target travelling at the current orientation
    double error_x = line_error.getX() + line_error.getY() * tan(nearAngle(tracking.g_pos.a, line_angle));
    // Only corrects if necessary (if robot won't land within an acceptable distance from the target)
    double correction = fabs(error_x) > end_error_x? kP_a * error_a: 0.0;
    printf("power_y: %lf, error_x: %lf, error_a: %lf\n", power_y, error_x, radToDeg(error_a));
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
    delay(10);
  }
  // while(false); // runs once
  while(line_error.getY() > 0.5);
  switch(brake_mode){
    case E_Brake_Modes::none:
      break;
    case E_Brake_Modes::coast:
      moveDrive(0.0, 0.0);
      break;
    case E_Brake_Modes::brake:
      driveBrake();
  }
  driveBrake();
}

// Below lies old x-drive code
/*
void Tracking::supplyMinPower(const Position& error){
  // ensures that each axis gets enough power to move, if it isn't well within its range of error
  if (fabs(power.x) < min_move_power.x && fabs(error.x) > 0.3) power.x = sgn(power.x) * min_move_power.x;
  if (fabs(power.y) < min_move_power.y && fabs(error.y) > 0.3) power.y = sgn(power.y) * min_move_power.y;
  if (fabs(power.a) < min_move_power.x && fabs(error.a) > 3.0) power.a = sgn(power.a) * min_move_power.a;
}

void Tracking::scalePowers(uint8_t max_power, uint8_t min_angle_power){
  double total_power = fabs(power.x) + fabs(power.y) + fabs(power.a);
  if(total_power > max_power){
    double power_scalar = max_power / total_power;
    double pre_scaled_power_a = fabs(power.a);
    power.x *= power_scalar,  power.y *= power_scalar, power.a *= power_scalar;
    // ensures drivebase gets enough angle power AFTER scaling
    guaranteeAnglePower(min_angle_power, pre_scaled_power_a, max_power);
  }
}

void Tracking::guaranteeAnglePower(uint8_t min_angle_power, uint8_t pre_scaled_power_a, uint8_t max_power){
  double power_xy = fabs(power.x) + fabs(power.y);
  double post_power_a;  // angle power after angle power guarantee process
  if(fabs(power.a) < min_angle_power && power_xy > 0){  // if angle power has been overshadowed by x and y
    // if angle power was less than min_angle_power before scaling, give angle power what it had before scaling
    if(pre_scaled_power_a < min_angle_power)  post_power_a = pre_scaled_power_a;
    else post_power_a = min_angle_power; // otherwise, give it min_angle_power
    double xy_scalar = (max_power - post_power_a)/power_xy;
    power.x *= xy_scalar, power.y *= xy_scalar, power.a = sgn(power.a) * post_power_a;
  }
}


void moveToTarget(Position target, E_Brake_Modes brake_mode, double max_power, double min_angle_power, double exit_power, bool overshoot, double end_error_d, double end_error_a){
  double error_a; // angular error 
  Vector error_pos(target - tracking.g_pos); // positional error

  PID pid_d(10.0, 0.0, 700.0, 0.0);  // pid for distance to target
  PID pid_a(135.0, 0.0, 0.0, 0.0);  // pid for angle

  // angle of line from starting position to target, from the vertical
  double line_angle = M_PI_2 - error_pos.getAngle();
  error_pos.rotate(line_angle); 
  // sign of distance to target along line from starting position to target
  int8_t start_sgn_line_error_y = sgn(error_pos.getY()), sgn_line_error_y;
  const double decel_dist = max_power/pid_d.getProportional(); // distance at which robot stops going full speed

  while(true){
    // obtains error in angle and error in position (global x and y)
    error_a = degToRad(target.a) - tracking.g_pos.a;
    error_pos = target - tracking.g_pos;
    tracking.drive_error = error_pos.getMagnitude();  // distance to target

    error_pos.rotate(line_angle); // now represents displacement along the line from starting position to target
    sgn_line_error_y = sgn(error_pos.getY());

    error_pos.rotate(tracking.g_pos.a - line_angle); // now represents local errors

    // computes PIDs' and saves them into the power position vector
    // if exit power is enabled, map the power from max_power to exit_power, otherwise use the PID like normal
    if(exit_power)  tracking.power = Vector(mapValues(tracking.drive_error, 0.0, decel_dist, exit_power, max_power), error_pos.getAngle(), E_Vector_Types::POLAR);
    else tracking.power = Vector(pid_d.compute(-tracking.drive_error, 0.0), error_pos.getAngle(), E_Vector_Types::POLAR);
  
    tracking.power.a = pid_a.compute(-error_a, 0.0);

    lcd::print(3, "power| x:%.2lf, y:%.2lf, a:%.2lf", tracking.power.x, tracking.power.y, tracking.power.a);

    tracking.supplyMinPower(Position(error_pos, error_a)); // if any axis has less than the min power, give it min power 
    tracking.scalePowers(max_power, min_angle_power); // scales powers so that the drivebase doesn't travel faster than max_power

    // exit conditions
    if((overshoot && sgn_line_error_y != start_sgn_line_error_y) || (tracking.drive_error < end_error_d && fabs(radToDeg(error_a)) < end_error_a)){
      switch(brake_mode){
        case E_Brake_Modes::none:
          break;
        case E_Brake_Modes::coast:
          moveDrive(0.0, 0.0, 0.0);
          break;
        case E_Brake_Modes::brake:
          driveBrake();
      }
      return;
    }
    moveDrive(tracking.power.x, tracking.power.y, tracking.power.a);
    delay(10);
  }
}
*/