#pragma once
#include "main.h"
#include <unordered_map>
using namespace pros;
using namespace pros::c;


class TaskEndException: public std::exception{
public:
  const char* what();
};

enum class notify_types_2{
  none, // no notification received
  interrupt,  // kills the task safely
  suspend,  // suspends the task
  resume,  // resumes the task
};



class _Task_{
  task_t task_handle = 0; // handle to the pros task
  const char* name;
  uint8_t id; // an int to represent the id of the task 

  bool isAlive();
  static std::unordered_map<task_t, _Task_*> task_handles;
  void notify_handle(); // performs action based on notification value

public:
  _Task_(const char* name = "");
  void start(task_fn_t function, void* parameters = nullptr, uint8_t prio = TASK_PRIORITY_DEFAULT, uint16_t stack_depth = TASK_STACK_DEPTH_DEFAULT);
  void killUnsafe(); // removes task from scheduler
  void kill();  // sends a notification to kill the task
  void suspend();  // sends a notification to suspend the task
  void resume();  // sends a notification to resume the task
  
  static void delay(uint32_t delay_time = 10);  // special delay that raises TaskEndException upon receiving interrupt notification
  static _Task_* get_task_from_task_handle(task_t task_handle);  // return _Task_ given a handle to a pros task

};
