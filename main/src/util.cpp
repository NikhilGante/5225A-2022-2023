#include "util.hpp"
#include <numbers>

double operator "" _deg(long double degree){
  return degToRad(degree);
}
double operator "" _rad(long double radians){
  return radToDeg(radians);
}

double operator "" _rot(long double rotations){
  return rotations * 2*std::numbers::pi;
}

double degToRad(double deg){
  return deg / 180.0 * std::numbers::pi;
}

double radToDeg(double rad){
  return rad / std::numbers::pi * 180.0;
}

double nearAngle(double angle, double reference){
	return round((reference - angle)/(2*std::numbers::pi)) * (2*std::numbers::pi) + angle - reference;
}