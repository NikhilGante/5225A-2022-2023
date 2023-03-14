#include "../util.hpp"
#include "geometry.hpp"

// position constructors
Position::Position(): x{0.0}, y{0.0}, a{0.0}{}
Position::Position(const Vector& point): x{point.getX()}, y{point.getY()}, a{0.0}{}
Position::Position(const Vector& point, double angle): x{point.getX()}, y{point.getY()}, a{angle}{}
Position::Position(double x, double y, double angle): x{x}, y{y}, a{angle}{}

// Position operator overloads
Vector Position::operator +(const Position& p2) const {return Vector(x + p2.x, y + p2.y);}

Vector Position::operator -(const Position& p2) const {return Vector(x - p2.x, y - p2.y);}


// vector methods
Vector::Vector(const double param_1, const double param_2, E_Vector_Types type){
  if (type == E_Vector_Types::POLAR)  setPolar(param_1, param_2);
  else  setCartesian(param_1, param_2);
}

Vector::Vector(const Position& position) {setCartesian(position.x, position.y);}

// configures point as cartesian vector, with x and y coordinates
void Vector::setCartesian(const double x, const double y){
  this-> x = x, this-> y = y;
  magnitude = sqrt(pow(x, 2) + pow(y, 2)), angle = atan2(y, x);
}

// configures point as polar vector, with an angle and magnitude
void Vector::setPolar(const double magnitude, const double angle){
  this-> magnitude = magnitude, this-> angle = angle;
  x = magnitude * cos(angle), y = magnitude * sin(angle);
}

// configures point as polar vector, with an angle and magnitude
void Vector::rotate(const double rotation_angle){
  setPolar(magnitude, angle + rotation_angle);
}

// getters
double Vector::getX() const {return this-> x;}
double Vector::getY() const {return this-> y;}
double Vector::getMagnitude() const {return this-> magnitude;}
double Vector::getAngle() const {return this-> angle;}

// Vector operator overloads
Vector Vector::operator +(const Vector& p2) const {return Vector(x + p2.getX(), y + p2.getY(), E_Vector_Types::CARTESIAN);}

Vector Vector::operator -(const Vector& p2) const {return Vector(x - p2.getX(), y - p2.getY(), E_Vector_Types::CARTESIAN);}