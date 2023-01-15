#include "util.hpp"
#include <cmath>

double operator "" _deg(long double degree){
  return degToRad(degree);
}
double operator "" _rad(long double radians){
  return radToDeg(radians);
}

double operator "" _rot(long double rotations){
  return rotations * rot_to_rad;
}

double degToRad(double deg){
  return deg * deg_to_rad;
}

double radToDeg(double rad){
  return rad * rad_to_deg;
}

double nearAngle(double angle, double reference){
	return std::round((reference - angle)/(2*std::numbers::pi)) * (2*std::numbers::pi) + angle - reference;
}