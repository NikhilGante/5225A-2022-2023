#pragma once
#include "main.h"
#include <iostream>

#define LOGS

using namespace pros;
using namespace std;
//forward declarations
class Timer;

#define WAIT_UNTIL(condition) while(pros::delay(10), !(condition));


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

/**
 * @brief checks whether a value is in range
 * 
 * @param value value to be checked
 * @param minimum range minimum value
 * @param maximum range maximum value
 * @return whether value is between minimum and maximum. will work even if maximum < minimum
 */
template <typename T>
bool inRange(T value, T minimum, T maximum){
  return (minimum <= value && value <= maximum) || (maximum <= value && value <= minimum);
}

//degrees to radians
double operator "" _deg(long double degree);

//radians to degrees
double operator "" _rad(long double radians);

//rotations to radians
double operator "" _rot(long double rotations);

/**
 * @brief converts radians to degrees
 * 
 * @param rad 
 * @return degrees
 */
double radToDeg(double rad);

/**
 * @brief converts degrees to radians
 * 
 * @param deg 
 * @return radians
 */
double degToRad(double deg);

/**
 * @brief returns the closest equivalent angle to refrence in radians
 * 
 * @param angle 
 * @param reference 
 * @return double 
 */
double nearAngle(double angle, double reference);

int random_direction();

// gets the sign of a value (0, 1 or -1)
template <typename T>
int sgn(T value){
  return (T(0) < value) - (value < T(0));
}

template <typename T>
bool contains(T& container, typename T::value_type item){
  return std::find(container.begin(), container.end(), item) != container.end();
}

// maps a value to a range
template <typename T>
T mapValues(T x, T in_min, T in_max, T out_min, T out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
// base case for recrusive function map_set
template <typename T>
T mapSet(T input, T in_min, T in_max, T out_min, T out_max, T range, T val){
  if (input <= range) return map(input, in_min, range, out_min, val);
  else {
    printf("INVALID INPUT IN MAP FUNCTION");
    return static_cast<T>(0);
  }
}
// maps a values to a set of maps (a piecewise function)
template <typename T, typename... Ts>
T mapSet(T input, T in_min, T in_max, T out_min, T out_max, T range1, T val_1, Ts... args){
  if (input <= range1) return map(input, in_min, range1, out_min, val_1);
  else return mapSet(input, range1, in_max, val_1, out_max, args...);
}