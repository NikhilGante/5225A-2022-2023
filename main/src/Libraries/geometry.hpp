#pragma once

class Vector; // forward declares Vector class

struct Position{   // stores members variables for a coordinate: x, y and angle
  double x, y, a;
  Position(); // default constructor
  Position(const Vector& point);
  Position(const Vector& point, double angle);
  Position(double x, double y, double angle);
   
  // arithmetic operator overloads
  Vector operator +(const Position& p2) const;
  Vector operator -(const Position& p2) const;
};

enum class E_Vector_Types {CARTESIAN, POLAR};

class Vector{

    double x, y, magnitude, angle;

  public:
    // constructors
    Vector(const double param_1, const double param_2, E_Vector_Types type = E_Vector_Types::CARTESIAN);
    Vector(const Position& position);

    // configures vector as cartesian vector, with x and y coordinates
    void setCartesian(const double x, const double y);

    // configures vector as polar vector, with an angle and magnitude
    void setPolar(const double magnitude, const double direction);

    void rotate(const double rotation_angle);

    // getters
    double getX() const;
    double getY() const;
    double getMagnitude() const;
    double getAngle() const;

    // arithmetic operator overloads
    Vector operator +(const Vector& p2) const;
    Vector operator -(const Vector& p2) const;
};