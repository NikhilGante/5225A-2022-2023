#include "tracking.hpp"
#include "config.hpp"

_Task update_t(update, "Tracking");

Tracking tracking;

void update(void* params){
  _Task* ptr = _Task::get_obj(params);
  Timer data_timer{"tracking logs"};
  // LeftEncoder.reset(); RightEncoder.reset(); BackEncoder.reset();
  
  double DistanceLR = 9.91, DistanceB = 0.85;
  double Left, Right, Back, NewLeft, NewRight, NewBack, LastLeft = LeftEncoder.get_value()/360.0 *(2.75*M_PI), LastRight =  RightEncoder.get_value()/360.0 *(2.75*M_PI), LastBack = BackEncoder.get_value()/360.0 *(2.75*M_PI);
  double Theta = 0.0, Beta = 0.0, Alpha = 0.0;
  double RadiusR, RadiusB, h, h2;
  double Xx, Xy, Yy, Yx;
  double last_x = 0, last_y = 0, last_a = 0;
  double last_vel_l = 0, last_vel_r = 0, last_vel_b = 0;

  Position last_position; // last position of robot
  uint32_t last_velocity_time = 0;    // time of last velocity update
  uint32_t velocity_update_time = 0; // time SINCE last velocity update


  while(true){
    NewLeft = LeftEncoder.get_value()/360.0 *(2.75*M_PI);
    NewRight = RightEncoder.get_value()/360.0 *(2.75*M_PI);
    NewBack = BackEncoder.get_value()/360.0 *(2.75*M_PI);

    // printf2("l: %d, r: %d ", NewLeft, NewRight);
    // printf2("l: %d, r: %d ", LeftEncoder.get_value(), RightEncoder.get_value());
    // printf2("x: %.2lf, y: %.2lf, a: %.2lf", millis(), tracking.g_position.x, tracking.g_position.y, rad_to_deg(tracking.g_position.angle));



    Left = NewLeft - LastLeft;
    Right = NewRight - LastRight;
    Back = NewBack - LastBack;
    // Back = 0;


    velocity_update_time = millis() - last_velocity_time;
    if(velocity_update_time > 10){  // velocity is updated every 20 ms
        tracking.l_velo = (NewLeft - last_vel_l) / velocity_update_time * 1000; // velocities are in inches per second
        tracking.r_velo = (NewRight - last_vel_r) / velocity_update_time * 1000;
        tracking.b_velo = (NewBack - last_vel_b) / velocity_update_time * 1000;

        last_vel_l = NewLeft;
        last_vel_r = NewRight;
        last_vel_b = NewBack;

        // gives us linear velocity in inches per second, and angular velocity in radians per second
        tracking.g_velocity.x = (tracking.g_position.x - last_position.x) / velocity_update_time * 1000;
        tracking.g_velocity.y = (tracking.g_position.y - last_position.y) / velocity_update_time * 1000;
        tracking.g_velocity.angle = (tracking.g_position.angle - last_position.angle) / velocity_update_time * 1000;

        last_position = {tracking.g_position.x, tracking.g_position.y, tracking.g_position.angle};

        last_velocity_time = millis();
    }

    LastLeft = NewLeft;
    LastRight = NewRight;
    LastBack = NewBack;

    Theta = (Left-Right)/DistanceLR;
    if (Theta != 0){
      RadiusR = Right/Theta;
      RadiusB = Back/Theta;
      Beta = Theta/2.0;
      h = (RadiusR + DistanceLR/2) *2 *sin(Beta);
      h2 = (RadiusB + DistanceB) *2 *sin(Beta);
    }
    else{
      h = Right;
      h2 = Back;
      Beta =0.0;
    }
    Alpha = tracking.g_position.angle + Beta;

    Xx = h2 * cos(Alpha);
    Xy = h2 * -sin(Alpha);
    Yx = h * sin(Alpha);
    Yy = h * cos(Alpha);

    tracking.g_position.x += Xx + Yx;
    tracking.g_position.y += Yy + Xy;
    tracking.g_position.angle += Theta;

    if(data_timer.get_time() > 1000){
      printf("%d || x: %.2lf, y: %.2lf, a: %.2lf\n", millis(), tracking.g_position.x, tracking.g_position.y, rad_to_deg(tracking.g_position.angle));
      printf("ENCODER L: %d, R: %d, B:%d \n", millis(), LeftEncoder.get_value(), RightEncoder.get_value(), BackEncoder.get_value());

      data_timer.reset();
    }
    /*
    tracking_data.print(&data_timer, 10, {
      [=](){return Data::to_char("%d || x: %.2lf, y: %.2lf, a: %.2lf\n", millis(), tracking.g_position.x, tracking.g_position.y, rad_to_deg(tracking.g_position.angle));},
      // [=](){return Data::to_char("%d || GLOBAL VELOCITY| x: %.2f, y: %.2f a: %.2f\n", millis(), tracking.g_velocity.x, tracking.g_velocity.y, rad_to_deg(tracking.g_velocity.angle));},
      // [=](){return Data::to_char("%d || ENCODER L: %d, R: %d, B:%d \n", millis(), LeftEncoder.get_value(), RightEncoder.get_value(), BackEncoder.get_value());},
      [=](){return Data::to_char("%d || ENCODER VELO| l: %.2f, r: %.2f, b: %.2f\n", millis(), tracking.l_velo, tracking.r_velo, tracking.b_velo);}
      // [=](){return Data::to_char("%d || accel| x: %.2f, y: %.2f, g: %.2f\n", millis(), , tracking.r_velo, tracking.b_velo);}
      // [=](){return Data::to_char("%d || %.2lf, %.2lf, %.2lf, %.2f, %.2f \n", millis(), tracking.g_position.x, tracking.g_position.y, rad_to_deg(tracking.g_position.angle), tracking.l_velo, tracking.b_velo);},
    });
    */

    if(ptr->notify_handle())return;
    delay(10);
  }
}
