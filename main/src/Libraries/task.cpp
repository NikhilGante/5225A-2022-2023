#include "task.hpp"
#include "timer.hpp"
#include "../util.hpp"

using namespace c;

_Task::_Task(std::string name): name{name} {}

bool _Task::isAlive() const {return task_handle && task_get_state(task_handle) != TASK_STATE_DELETED;}

void _Task::killUnsafe(){
  if(isAlive()){
    task_delete(task_handle); // remove the task from scheduler if it's running
    system_log("%s Task unsafely killed", name);
  }
  else system_log("%s Task already dead, killUnsafe failed", name);
}

void _Task::kill(){
  if(isAlive()){
    // sends a notification to kill the task
    task_notify_ext(task_handle, static_cast<int>(notify_types_2::interrupt), NOTIFY_ACTION_OWRITE, nullptr);
    system_log("%s Task killed", name);
    if(task_get_state(task_handle) == TASK_STATE_SUSPENDED) resume();
  }
  else system_log("%s Task already dead, kill failed", name);
}

void _Task::suspend(){
  if(isAlive()){
    task_notify_ext(task_handle, static_cast<int>(notify_types_2::suspend), NOTIFY_ACTION_OWRITE, nullptr); // sends a notification to kill task
    WAIT_UNTIL(task_get_state(task_handle) == TASK_STATE_SUSPENDED); // wait for the task to suspend
    system_log("%s Task suspended", name);
  }
  else system_log("%s Task already dead, suspend failed", name);
}

void _Task::resume(){
  // attempts to resume only if the task has started, otherwise a resume would block the program
  if(task_handle){
    task_resume(task_handle);
    system_log("%s Task resumed", name);
  }
  else system_log("%s | resume failed, task not started yet", name);
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
        system_log(term_colours::BLUE, "%s interrupted [inside function]", task_get_name(current_task));
        throw TaskEndException{};
        break;
      case notify_types_2::suspend:
        system_log(term_colours::BLUE, "%s suspended [inside function]", task_get_name(current_task));
        task_suspend(current_task);
        break;
    }
    // ensures to sleep at most 10ms, or time left if that's less than 10ms 
    int time_left = delay_time - delay_timer.getTime();
    pros::delay(std::min(time_left < 0 ? 0 : time_left, 10));
  }
  while(delay_timer.getTime() < delay_time);
}