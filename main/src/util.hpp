#pragma once
#include "main.h"

// #include "okapi/api/util/mathUtil.hpp"
#include <numbers>

inline constexpr double rot_to_deg = 360;
inline constexpr double rot_to_rad = 2*std::numbers::pi;
inline constexpr double deg_to_rad = rot_to_rad/rot_to_deg;
inline constexpr double rad_to_deg = 1/deg_to_rad;
inline constexpr double deg_to_rot = 1/rot_to_deg;
inline constexpr double rad_to_rot = 1/rot_to_rad;

#define WAIT_UNTIL(condition) while(pros::delay(10), !(condition)) //! DO NOT ADD A SEMICOLON
//* It's pros::delay because a macro replacement may be in different scopes that have Task as more visible than pros

#ifndef __FILENAME__
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif
//! Don't use DEBUG before initialize
#define DEBUG printf("Time:%07d - File:%s | Function:%s | Line:%d\n", millis(), __FILENAME__, __PRETTY_FUNCTION__, __LINE__);
#define STRINGIFY(...) #__VA_ARGS__
#define PRINT(...)  std::cout __VA_OPT__(<< '\'' << STRINGIFY(__VA_ARGS__) << "\' = \'" << (__VA_ARGS__) << '\'' )<< '\n';

constexpr bool inRangeIncl(double value, double minimum, double maximum) {return (minimum <= value && value <= maximum) || (maximum <= value && value <= minimum);}
constexpr bool inRangeExcl(double value, double minimum, double maximum) {return (minimum < value && value < maximum) || (maximum < value && value < minimum);}

constexpr double degToRad(double deg) {return deg * deg_to_rad;}
constexpr double radToDeg(double rad) {return rad * rad_to_deg;}

constexpr double operator "" _deg(long double degree) {return degToRad(degree);}
constexpr double operator "" _rad(long double radians) {return radToDeg(radians);}
constexpr double operator "" _rot(long double rotations) {return rotations * rot_to_rad;}

constexpr double nearAngle(double angle, double reference) {return std::round((reference - angle)/(2*std::numbers::pi)) * (2*std::numbers::pi) + angle - reference;}

constexpr int sgn(double x) {return x == 0 ? 0 : x > 0 ? 1 : -1;}

constexpr double remapRange(const double value,
                            const double oldMin,
                            const double oldMax,
                            const double newMin,
                            const double newMax) {
  return (value - oldMin) * ((newMax - newMin) / (oldMax - oldMin)) + newMin;
}

// base case for recursive function mapSet
auto mapSet(auto input, auto in_min, auto in_max, auto out_min, auto out_max, auto range, auto val){
  if (input <= range) return remapRange(input, in_min, range, out_min, val);
  else throw std::domain_error("INVALID INPUT IN MAP FUNCTION");
}
// maps a values to a set of maps (a piecewise function)
auto mapSet(auto input, auto in_min, auto in_max, auto out_min, auto out_max, auto range1, auto val_1, auto... args){
  if (input <= range1) return remapRange(input, in_min, range1, out_min, val_1);
  else return mapSet(input, range1, in_max, val_1, out_max, args...);
}