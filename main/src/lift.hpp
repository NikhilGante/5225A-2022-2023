#pragma once
#include "main.h"
#include "Libraries/async.hpp"
#include "Libraries/task.hpp"

extern int lift_index;
extern int lift_arr[5];
void liftMoveToTargetFn(void* params);

struct LiftMoveToTargetParams{
  LiftMoveToTargetParams(int lift_index): lift_index(lift_index){}
  int lift_index;
};
extern _Task lift_task;
extern AsyncObj<LiftMoveToTargetParams> liftMoveToTarget;