#pragma once
#include "Libraries/logging.hpp"
#include <cmath>
#include <numbers>

inline constexpr double rot_to_deg = 360;
inline constexpr double rot_to_rad = 2*std::numbers::pi;
inline constexpr double deg_to_rad = rot_to_rad/rot_to_deg;
inline constexpr double rad_to_deg = 1/deg_to_rad;
inline constexpr double deg_to_rot = 1/rot_to_deg;
inline constexpr double rad_to_rot = 1/rot_to_rad;

#define LOGS

#define WAIT_UNTIL(condition) while(pros::delay(10), !(condition)) //! DO NOT ADD A SEMICOLON

// cycle check macro (checks a condition for a specified amount of cycles)
#define CYCLE_CHECK(exit_condition, checkCount, delayTime) \
{\
  int successCount = 0;\
  while (successCount < checkCount){\
    if (exit_condition) successCount++;\
    else successCount = 0;\
    _Task::delay(delayTime);\
  }\
}
constexpr bool inRange(double value, double minimum, double maximum) {return (minimum <= value && value <= maximum) || (maximum <= value && value <= minimum);}

constexpr double degToRad(double deg) {return deg * deg_to_rad;}
constexpr double radToDeg(double rad) {return rad * rad_to_deg;}

constexpr double operator "" _deg(long double degree) {return degToRad(degree);}
constexpr double operator "" _rad(long double radians) {return radToDeg(radians);}
constexpr double operator "" _rot(long double rotations) {return rotations * rot_to_rad;}

constexpr double nearAngle(double angle, double reference) {return std::round((reference - angle)/(2*std::numbers::pi)) * (2*std::numbers::pi) + angle - reference;}

constexpr int sgn(double x) {return x == 0 ? 0 : x > 0 ? 1 : -1;}

// maps a value to a range
auto mapValues(auto x, auto in_min, auto in_max, auto out_min, auto out_max) {return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;}
// base case for recursive function mapSet
auto mapSet(auto input, auto in_min, auto in_max, auto out_min, auto out_max, auto range, auto val){
  if (input <= range) return map(input, in_min, range, out_min, val);
  else {
    misc("INVALID INPUT IN MAP FUNCTION");
    return 0;
  }
}
// maps a values to a set of maps (a piecewise function)
auto mapSet(auto input, auto in_min, auto in_max, auto out_min, auto out_max, auto range1, auto val_1, auto... args){
  if (input <= range1) return map(input, in_min, range1, out_min, val_1);
  else return mapSet(input, range1, in_max, val_1, out_max, args...);
}