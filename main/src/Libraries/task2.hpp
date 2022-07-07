#pragma once
#include "main.h"
#include <unordered_map>
using namespace pros;
using namespace pros::c;

int increment_controller_line();

class TaskEndException: public std::exception{
public:
  const char* what();
};

enum class notify_types_2{
  none, // no notification received
  interrupt,  // kills the task safely
  suspend,  // suspends the task
};

extern int controller_line_counter;

class _Task_{
  task_t task_handle = 0; // handle to the pros task
  const char* name;

  bool isAlive(); // returns if the task is currently on the scheduler

public:
  _Task_(const char* name = "");
  void start(task_fn_t function, void* parameters = nullptr, uint8_t prio = TASK_PRIORITY_DEFAULT, uint16_t stack_depth = TASK_STACK_DEPTH_DEFAULT);
  void killUnsafe(); // removes task from scheduler
  void kill();  // sends a notification to kill the task
  void suspend();  // sends a notification to suspend the task
  void resume();  // resume the task
  
  static void delay(uint32_t delay_time = 10);  // special delay that handles notifications

};
