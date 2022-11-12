#pragma once
#include "main.h"
#include <cmath>




int sgn(double x);
std::tuple<double, double, double> findCircle(double x1, double y1, double x2, double y2, double x3, double y3);
std::pair<double, double> getPoint(double x1, double y1, double r, double a1);