#include <cmath>
#include "config.hpp"
#include "tracking.hpp"
#include "Libraries/pid.hpp"
#include "Libraries/timer.hpp"
#include "util.hpp"
#include "drive.hpp"

Tracking tracking; // singleton tracking object


void tracking_update(){
  // LeftEncoder.reset(); RightEncoder.reset(); BackEncoder.reset();
  double dist_lr = 13.80, dist_b = 6.75;  // distance between left and right tracking wheels, and distance from back wheel to tracking centre
  double left, right, back, new_left, new_right, new_back, last_left = LeftEncoder.get_value()/360.0 *(2.75*M_PI), last_right = RightEncoder.get_value()/360.0 *(2.75*M_PI), last_back = BackEncoder.get_value()/360.0 *(2.75*M_PI);
  double theta = 0.0, beta = 0.0, alpha = 0.0;
  double radius_r, radius_b, h_y, h_x;
  double x_x, x_y, y_y, y_x;
  double last_x = 0, last_y = 0, last_a = 0;
  double last_vel_l = 0, last_vel_r = 0, last_vel_b = 0;

  double sin_alpha, cos_alpha, sin_beta; // locals to avoid duplicate computations

  Position last_position; // last position of robot
  Timer velocity_timer{"velocity_timer"};
  uint32_t velocity_update_time;  // time since last velocity update

  while(true){
    new_left = LeftEncoder.get_value()/360.0 *(2.75*M_PI);
    new_right = RightEncoder.get_value()/360.0 *(2.75*M_PI);
    new_back = BackEncoder.get_value()/360.0 *(2.75*M_PI);

    // updates how much each side of the robot travelled in inches since the le(left, right and back)
    left = new_left - last_left;
    right = new_right - last_right;
    back = new_back - last_back;

    if(velocity_timer.get_time() > 20){  // velocity is updated every 20 
      velocity_update_time = velocity_timer.get_time();
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

    // printf("L:%d R:%d B:%d", LeftEncoder.get_value(), RightEncoder.get_value(), BackEncoder.get_value());
		pros::lcd::print(0, "L:%d R:%d B:%d", LeftEncoder.get_value(), RightEncoder.get_value(), BackEncoder.get_value());
		pros::lcd::print(1, "x:%lf y:%lf a:%lf", tracking.g_pos.x, tracking.g_pos.y, (tracking.g_pos.a)*180/M_PI);

    delay(10);
  }
}



void moveToTarget(Position target, brake_modes brake_mode, double max_power, double exit_power, bool overshoot, double end_error_d, double end_error_a){
  double error_a; // angular error 
  Vector error_pos(target - tracking.g_pos); // positional error
  double error_d; // distance to target;

  PID pid_d(10.0, 0.0, 1000.0, 0.0);  // pid for distance to target
  PID pid_a(135.0, 0.0, 0.0, 0.0);  // pid for angle

  // angle of line from starting position to target, from the vertical
  double line_angle = M_PI_2 - Vector(target - tracking.g_pos).getAngle();
  error_pos.rotate(line_angle); 
  // sign of distance to target along line from starting position to target
  int8_t start_sgn_line_error_y = sgn(error_pos.getY()), sgn_line_error_y;

  Position power; // power to apply to the drive motors
  double total_power; // sum of the absulute values of each of the power
  double power_scalar; // max_power / total_power
  Position min_move_power(15.0, 15.0, 10.0);  

  while(true){
    // obtains error in angle and error in position (global x and y)
    error_a = deg_to_rad(target.a) - tracking.g_pos.a;
    error_pos = target - tracking.g_pos;

    error_pos.rotate(line_angle); // now represents displacement along the line from starting position to target
    sgn_line_error_y = error_pos.getY();

    error_pos.rotate(tracking.g_pos.a - line_angle); // now represents localc errors

    error_d = error_pos.getMagnitude();

    // computes PIDs' and saves them into the power position vector
    power = Vector(pid_d.compute(-error_d, 0.0), error_pos.getAngle(), vector_types::POLAR);
    power.a = pid_a.compute(-error_a, 0.0);
    lcd::print(3, "power| x:%.2lf, y:%.2lf, a:%.2lf", power.x, power.y, power.a);
    // if any axis has less than the min power, give it min power 
    if (fabs(power.x) < min_move_power.x) power.x = sgn(power.x) * min_move_power.x;
    if (fabs(power.y) < min_move_power.y) power.y = sgn(power.y) * min_move_power.y;
    if (fabs(power.a) < min_move_power.x) power.a = sgn(power.a) * min_move_power.a;
    
    total_power = fabs(power.x) + fabs(power.y) + fabs(power.a);
    // scales powers so that the drivebase doesn't travel faster than max_power
    if(total_power > max_power){
      power_scalar = max_power / total_power;
      power.x *= power_scalar;
      power.y *= power_scalar;
      power.a *= power_scalar;
    }

    // exit conditions
    if((overshoot && sgn_line_error_y != start_sgn_line_error_y) || 
    (error_d < end_error_d && fabs(rad_to_deg(error_a)) < end_error_a)){
      switch(brake_mode){
        case brake_modes::none:
          break;
        case brake_modes::coast:
          moveDrive(0.0, 0.0, 0.0);
          break;
        case brake_modes::brake:
          drive_brake();
      }
      return;
    }
    moveDrive(power.x, power.y, power.a);
    delay(10);
  }
}
