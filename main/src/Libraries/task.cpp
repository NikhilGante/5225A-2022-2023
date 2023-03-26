#include "task.hpp"
#include "../util.hpp"
#include "timer.hpp"
#include "../config.hpp"
#include "logging.hpp"


_Task::_Task(const char* name): name(name)
{}

// private method
bool _Task::isAlive(){
  return task_handle && task_get_state(task_handle) != E_TASK_STATE_DELETED;
}

void _Task::killUnsafe(){
  if(isAlive()) task_delete(task_handle); // remove the task from scheduler if it's running
  else printf("%s | Already dead, killUnsafe failed\n", name);
}

void _Task::kill(){
  if(isAlive()){
    // sends a notification to kill the task
    task_notify_ext(task_handle, (int)E_Notify_Types::interrupt, E_NOTIFY_ACTION_OWRITE, nullptr);
    if(task_get_state(task_handle) == E_TASK_STATE_SUSPENDED) resume();
  }
  else printf("%s | Already dead, kill failed\n", name);
}

void _Task::suspend(){
  if(isAlive()){
    task_notify_ext(task_handle, (int)E_Notify_Types::suspend, E_NOTIFY_ACTION_OWRITE, nullptr); // sends a notification to kill task
    WAIT_UNTIL(task_get_state(task_handle) == E_TASK_STATE_SUSPENDED); // wait for the task to suspend
  }
  else log("%s | suspend failed, task is dead\n", name);
}

void _Task::resume(){
  // attempts to resume only if the task has started, otherwise a resume would block the program
  if(task_handle) task_resume(task_handle);
  else log("%s | resume failed, task not started yet\n", name);
}

void _Task::delay(uint32_t delay_time){
  Timer delay_timer{"task_delay_timer"};
  do {
    task_t current_task = task_get_current();
    // Handle notifications from current task
    switch((E_Notify_Types)task_notify_take(true, 0)){
      case E_Notify_Types::none:
        break;
      case E_Notify_Types::interrupt:
        printf("%s interrupted\n", task_get_name(current_task));
        throw TaskEndException{};
        break;
      case E_Notify_Types::suspend:
        printf("%s suspended\n", task_get_name(current_task));
        task_suspend(current_task);
        break;
    }
    // Ensures to sleep at most 10ms, or time left if that's less than 10ms 
    int32_t time_left = delay_time - delay_timer.getTime();
    pros::delay(min<int32_t>(time_left < 0 ? 0: time_left, 10l));
  }
  while(delay_timer.getTime() < delay_time);
}


void _Task::delayUntil(std::uint32_t& prev_time, uint32_t delta, const char* overflow_log){
  int32_t time_left = prev_time+delta-millis();

  if (time_left < 0){
    log("%s | cycle took: %d, should have taken at most: %d\n", overflow_log, delta-time_left, delta);
  }

  do {
    task_t current_task = task_get_current();
    // Handle notifications from current task
    switch((E_Notify_Types)task_notify_take(true, 0)){
      case E_Notify_Types::none:
        break;
      case E_Notify_Types::interrupt:
        printf("%s interrupted\n", task_get_name(current_task));
        throw TaskEndException{};
        break;
      case E_Notify_Types::suspend:
        printf("%s suspended\n", task_get_name(current_task));
        task_suspend(current_task);
        break;
    }
    // Ensures to sleep at most 10ms, or time left if that's less than 10ms 
    time_left = prev_time+delta-millis();
    pros::delay(min<int32_t>(time_left < 0 ? 0: time_left, 10l));
  } while(time_left > 0);

  prev_time = millis();
}