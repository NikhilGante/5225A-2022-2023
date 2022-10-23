#include "util.hpp"
#include <cmath>

double operator "" _deg(long double degree){
  return degToRad(degree);
}
double operator "" _rad(long double radians){
  return radToDeg(radians);
}

double operator "" _rot(long double rotations){
  return rotations * 2*M_PI;
}

double degToRad(double deg){
  return deg / 180.0 * M_PI;
}

double radToDeg(double rad){
  return rad / M_PI * 180.0;
}

double nearAngle(double angle, double reference){
	return round((reference - angle)/(2*M_PI)) * (2*M_PI) + angle - reference;
}

