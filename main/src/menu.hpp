#pragma once
#include "auton.hpp"
#include <functional>
using namespace std;

class Auton{
private:

public:
  Auton(const char* name, function<void()> func);
};
