#include "util.hpp"
#include <cmath>

double operator "" _deg(long double degree){
  return deg_to_rad(degree);
}
double operator "" _rad(long double radians){
  return rad_to_deg(radians);
}

double operator "" _rot(long double rotations){
  return rotations * 2*M_PI;
}

double deg_to_rad(double deg){
  return deg / 180.0 * M_PI;
}

double rad_to_deg(double rad){
  return rad / M_PI * 180.0;
}

double near_angle(double angle, double reference){
	return round((reference - angle)/(2*M_PI)) * (2*M_PI) + angle - reference;
}