#include "task.hpp"
#include "timer.hpp"
#include "../util.hpp"

using namespace c;

_Task::_Task(std::string name): name{name} {}

// private method
bool _Task::isAlive(){
  return task_handle && task_get_state(task_handle) != TASK_STATE_DELETED;
}

void _Task::killUnsafe(){
  if(isAlive()) task_delete(task_handle); // remove the task from scheduler if it's running
  else task_log.print("%s | Already dead, killUnsafe failed", name);
}

void _Task::kill(){
  if(isAlive()){
    // sends a notification to kill the task
    task_notify_ext(task_handle, static_cast<int>(notify_types_2::interrupt), NOTIFY_ACTION_OWRITE, nullptr);
    if(task_get_state(task_handle) == TASK_STATE_SUSPENDED) resume();
  }
  else task_log.print("%s | Already dead, kill failed", name);
}

void _Task::suspend(){
  if(isAlive()){
    task_notify_ext(task_handle, static_cast<int>(notify_types_2::suspend), NOTIFY_ACTION_OWRITE, nullptr); // sends a notification to kill task
    WAIT_UNTIL(task_get_state(task_handle) == TASK_STATE_SUSPENDED); // wait for the task to suspend
  }
  else task_log.print("%s | suspend failed, task is dead", name);
}

void _Task::resume(){
  // attempts to resume only if the task has started, otherwise a resume would block the program
  if(task_handle) task_resume(task_handle);
  else task_log.print("%s | resume failed, task not started yet", name);
}

void _Task::delay(uint32_t delay_time){
  Timer delay_timer{"Task Delay Timer"};
  do{
    task_t current_task = task_get_current();
    // Handle notifications from current task
    switch(static_cast<notify_types_2>(task_notify_take(true, 0))){
      case notify_types_2::none:
        break;
      case notify_types_2::interrupt:
        task_log.print(term_colours::BLUE, "%s interrupted", task_get_name(current_task));
        throw TaskEndException{};
        break;
      case notify_types_2::suspend:
        task_log.print(term_colours::BLUE, "%s suspended", task_get_name(current_task));
        task_suspend(current_task);
        break;
    }
    // ensures to sleep at most 10ms, or time left if that's less than 10ms 
    int time_left = delay_time - delay_timer.getTime();
    pros::delay(std::min(time_left < 0 ? 0 : time_left, 10)); //?what if you wanted to delay 50. It would only delay 10?
  }
  while(delay_timer.getTime() < delay_time);
}