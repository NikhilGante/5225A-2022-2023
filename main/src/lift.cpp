#include "lift.hpp"
#include "config.hpp"
#include "Libraries/pid.hpp"

PID b_lift_pid = PID(2.0,0.0,0.0,0.0);
int lift_index = 0;


int lift_arr[5] = {1185, 1400, 1900, 2170, 2775};


// int output = b_lift_pid.compute(b_lift_pot.get_value(), lift_arr[lift_index]);
//     if(abs(output) < 40) output = 40 * sgn(output); // enforces a minimum of 30 power
//     b_lift_m.move(output);
//     if(fabs(b_lift_pid.get_error()) < 10){
//       printf("Finished move, %d\n", b_lift_pot.get_value());
//       b_lift_m.move_velocity(0);
//       return;
//     }
//     delay(10);