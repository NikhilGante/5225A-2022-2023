#include "Libraries/geometry.hpp"
#include "Libraries/task.hpp"


extern _Task update_t;
void update(void* params);

class Tracking{
  public:
    //Can we make x_coord, y_coord, global_angle a Position
    Position g_position;
    // double x_coord = 0.0; double y_coord = 0.0; double global_angle = 0.0; //double target_x = 0.0;
    double power_x, power_y, power_a;
    double l_velo, r_velo, b_velo;

    double drive_error;
    bool move_complete = true, move_started = false;
    int movetime = 0;

    //global g_velocity stores x, y and angular velocities
    Position g_velocity;
    void wait_for_dist(double distance, int timeout = 0);
    double get_angle_in_deg();
    void reset(double x, double y=0.0, double a=0.0);
    void reset(Position position = {});
    double get_dist(Position pos1);
    static double get_dist(Position pos1, Position pos2);
};

extern Tracking tracking;

// macros for convenient modification of move algorithms

#define polar_to_vector_line(start_x, start_y, magnitude, theta, angle)\
  {start_x, start_y}, {start_x + sin(deg_to_rad(theta)) * magnitude, start_y + cos(deg_to_rad(theta)) * magnitude, angle}\

#define polar_to_vector_facing_line(start_x, start_y, magnitude, angle){\
  polar_to_vector(start_x, start_y, magnitude, angle, angle)\
}
#define polar_to_vector_point(start_x, start_y, magnitude, theta, angle)\
  {start_x + sin(deg_to_rad(theta)) * magnitude, start_y + cos(deg_to_rad(theta)) * magnitude, angle}\

#define angle_to_point(x, y)\
  rad_to_deg(atan2(x - tracking.g_position.x, y - tracking.g_position.y))\