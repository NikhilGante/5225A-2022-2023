#include "vision.hpp"

_Vision::signature _Vision::blue_sig = Vision::signature_from_utility(1, -2835, -983, -1909, 6951, 9185, 8068, 3.0, 0);
_Vision::signature _Vision::red_sig  = Vision::signature_from_utility(2, 10795, 13423, 12109, -399, 809, 205, 3.0, 0);

_Vision::_Vision(int port): Vision (port, VISION_ZERO_CENTER){
  set_signature(blue_sig.id, &blue_sig);
  set_signature(red_sig.id, &red_sig);
}

void _Vision::print_object(object obj){
  device_log("Time:%d | Hor:%d, Ver:%d, Width:%d, Height:%d\n", millis(), obj.left_coord, obj.top_coord, obj.width, obj.height);
}

_Vision::object _Vision::blue_object() const {return get_by_sig(0, 1);}
bool _Vision::blue_exists()     const {return blue_object().signature != 255;}
int  _Vision::blue_width()      const {return blue_object().width;}
int  _Vision::blue_height()     const {return blue_object().height;}
int  _Vision::blue_vertical()   const {return blue_object().top_coord;}
int  _Vision::blue_horizontal() const {return blue_object().left_coord;}

_Vision::object _Vision::red_object() const {return get_by_sig(0, 2);}
bool _Vision::red_exists()     const {return red_object().signature != 255;}
int  _Vision::red_width()      const {return red_object().width;}
int  _Vision::red_height()     const {return red_object().height;}
int  _Vision::red_vertical()   const {return red_object().top_coord;}
int  _Vision::red_horizontal() const {return red_object().left_coord;}