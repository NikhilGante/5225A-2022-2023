#include "task2.hpp"
#include "../util.hpp"
#include "timer.hpp"

std::unordered_map<task_t, _Task_*> _Task_::task_handles;

_Task_::_Task_(const char* name): name(name)
{
}

// private method
bool _Task_::isAlive(){
  lcd::print(7, "hey%d", task_handle);

  return task_handle && task_get_state(task_handle) != E_TASK_STATE_DELETED;
}


void _Task_::start(task_fn_t function, void* parameters, uint8_t prio, uint16_t stack_depth){
  // kill(); // kills task if it's alive
  task_handles.erase(task_handle);  // deletes existing element
  task_handle = task_create(function, parameters, prio, stack_depth, name);
  lcd::print(5, "bro%d", task_handle);
  task_handles[task_handle] = this;
}

void _Task_::killUnsafe(){
  if(isAlive()){
    task_delete(task_handle); // remove the task from scheduler if it's running
    WAIT_UNTIL(task_get_state(task_handle) == E_TASK_STATE_DELETED); // wait for the task to die
  }
  else printf("%s | Already dead, killUnsafe failed\n", name);
}

void _Task_::kill(){
  if(isAlive()){
    // lcd::print(2, "isalive");
    Timer timer{"timer"};
    task_notify_ext(task_handle, (int)notify_types_2::interrupt, E_NOTIFY_ACTION_OWRITE, nullptr); // sends a notification to kill task
    WAIT_UNTIL(task_get_state(task_handle) == E_TASK_STATE_DELETED); // wait for the task to die
    lcd::print(7,"time%d", timer.get_time());
  }
  else  lcd::print(2, "dead");
  // else printf("%s | Already dead, kill failed\n", name);
}

void _Task_::suspend(){
  task_notify_ext(task_handle, (int)notify_types_2::suspend, E_NOTIFY_ACTION_OWRITE, nullptr); // sends a notification to kill task
  WAIT_UNTIL(task_get_state(task_handle) == E_TASK_STATE_SUSPENDED); // wait for the task to suspend
}

void _Task_::resume(){
  // task_notify_ext(task_handle, (int)notify_types_2::resume, E_NOTIFY_ACTION_OWRITE, nullptr); // sends a notification to kill task
  task_resume(task_handle);
}


void _Task_::notify_handle(){
  switch((notify_types_2)task_notify_take(true, 0)){
    case notify_types_2::none:
      break;
    case notify_types_2::interrupt:
      printf("%s interrupted", name);
      lcd::print(6, "thrown");
      throw TaskEndException{};
      break;
    case notify_types_2::suspend:
      printf("%s suspended", name);
      lcd::print(6, "suspended");
      task_suspend(task_handle);
      break;
    case notify_types_2::resume:
      lcd::print(6, "resumed");
      task_resume(task_handle);

      break;
  }
}

// static methods
void _Task_::delay(uint32_t delay_time){
  Timer delay_timer{"task_delay_timer"};
  while(delay_timer.get_time() < delay_time){
    get_task_from_task_handle(task_get_current())->notify_handle();  // handle notifications from current task
    pros::delay(10);
  }
}

_Task_* _Task_::get_task_from_task_handle(task_t task_handle){
  return task_handles[task_handle];
}
