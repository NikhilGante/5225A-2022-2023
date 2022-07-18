#include "tracking.hpp"
#include "config.hpp"
#include "Libraries/timer.hpp"

Tracking tracking; // singleton tracking object


void Tracking::update(){
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
      tracking.g_velocity.x = (g_position.x - last_position.x) / velocity_update_time * 1000;
      tracking.g_velocity.y = (g_position.y - last_position.y) / velocity_update_time * 1000;
      tracking.g_velocity.angle = (g_position.angle - last_position.angle) / velocity_update_time * 1000;

      last_position = {g_position.x, g_position.y, g_position.angle};

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
    alpha = g_position.angle + beta;  // angle of local x and y vectors
    sin_alpha = sin(alpha);
    cos_alpha = cos(alpha);

    x_x = h_x * cos_alpha; // global x movement detected by local x (back wheel) arc
    x_y = h_x * -sin_alpha; // global y movement detected by local x (back wheel) arc
    y_x = h_y * sin_alpha; // global x movement detected by local y (right wheel) arc
    y_y = h_y * cos_alpha; // global y movement detected by local y (right wheel) arc

    g_position.x += x_x + y_x;
    g_position.y += y_y + x_y;
    g_position.angle += theta;

    // printf("L:%d R:%d B:%d", LeftEncoder.get_value(), RightEncoder.get_value(), BackEncoder.get_value());
		pros::lcd::print(0, "L:%d R:%d B:%d", LeftEncoder.get_value(), RightEncoder.get_value(), BackEncoder.get_value());
		pros::lcd::print(1, "x:%lf y:%lf a:%lf", tracking.g_position.x, tracking.g_position.y, (tracking.g_position.angle)*180/M_PI);

    delay(10);
  }
}

