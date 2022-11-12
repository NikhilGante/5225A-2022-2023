#include "task.hpp"
#include "../util.hpp"
#include "timer.hpp"
#include "../config.hpp"
#include "logging.hpp"


_Task_::_Task_(const char* name): name{name} {}

// private method
bool _Task_::isAlive(){
  return task_handle && task_get_state(task_handle) != E_TASK_STATE_DELETED;
}


// void _Task_::start(task_fn_t function, void* parameters, uint8_t prio, uint16_t stack_depth){

// }

void _Task_::killUnsafe(){
  if(isAlive()) task_delete(task_handle); // remove the task from scheduler if it's running
  else printf("%s | Already dead, killUnsafe failed\n", name);
}

void _Task_::kill(){
  if(isAlive()){
    // sends a notification to kill the task
    task_notify_ext(task_handle, (int)notify_types_2::interrupt, E_NOTIFY_ACTION_OWRITE, nullptr);
    if(task_get_state(task_handle) == E_TASK_STATE_SUSPENDED) resume();
  }
  else printf("%s | Already dead, kill failed\n", name);
}

void _Task_::suspend(){
  if(isAlive()){
    task_notify_ext(task_handle, (int)notify_types_2::suspend, E_NOTIFY_ACTION_OWRITE, nullptr); // sends a notification to kill task
    WAIT_UNTIL(task_get_state(task_handle) == E_TASK_STATE_SUSPENDED); // wait for the task to suspend
  }
  else log("%s | suspend failed, task is dead\n", name);
}

void _Task_::resume(){
  // attempts to resume only if the task has started, otherwise a resume would block the program
  if(task_handle) task_resume(task_handle);
  else log("%s | resume failed, task not started yet\n", name);
}


// static methods
void _Task_::delay(uint32_t delay_time){
  Timer delay_timer{"task_delay_timer"};
  do {
    task_t current_task = task_get_current();
    // handle notifications from current task
    switch((notify_types_2)task_notify_take(true, 0)){
      case notify_types_2::none:
        break;
      case notify_types_2::interrupt:
        printf("%s interrupted", task_get_name(current_task));
        throw TaskEndException{};
        break;
      case notify_types_2::suspend:
        printf("%s suspended", task_get_name(current_task));
        task_suspend(current_task);
        break;
    }
    // ensures to sleep at most 10ms, or time left if that's less than 10ms 
    int32_t time_left = delay_time - delay_timer.get_time();
    pros::delay(min<int32_t>(time_left < 0 ? 0: time_left, 10l));
  }
  while(delay_timer.get_time() < delay_time);
}