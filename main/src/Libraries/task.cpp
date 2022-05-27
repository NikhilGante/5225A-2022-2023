#include "task.hpp"

_Task::_Task(pros::task_fn_t function, const char* name, void* params, std::uint32_t prio, std::uint16_t stack_depth){
  this->function = function;
  this->name = name;
  this->params = std::make_tuple(this,std::move(params));
  this->prio = prio;
  this->stack_depth = stack_depth;
}

_Task::~_Task(){
  printf("%s entered destructor\n", this->name);
  this->kill();
}

_Task* _Task::get_obj(void* params){
  //gets the _Task class pointer out of tuple passed as void*
  return std::get<0>(*(std::tuple<_Task*, void*>*)(params));
}

void* _Task::get_params(void* params){
  //gets void* params from the tuple
  return std::get<1>(*(std::tuple<_Task*, void*>*)(params));
}


pros::Task* _Task::get_task_ptr()const{
  return this->task_ptr;
}

// accepts regular params pointer
void _Task::start(void* params){
   printf("%s starting\n", this->name);
   if(this->task_ptr != NULL){
     printf("%s was already started", this->name);
     this->kill();
   }
   this->params = std::make_tuple(this,std::move(params));
   this->task_ptr = new pros::Task(this->function, &this->params, this->prio, this->stack_depth, this->name);
   printf("%s started\n", this->name);
}

void _Task::kill(){
  if(this->task_ptr != NULL){
    printf("%s killing", this->name);
    this->task_ptr->notify_ext((int)stop, E_NOTIFY_ACTION_OWRITE,NULL);
    printf("%s notified", this->name);
    wait_until(this->task_ptr->get_state() == E_TASK_STATE_DELETED){
      printf("%s state %d", this->name, this->task_ptr->get_state());
    }
    printf("%s state check passed\n", this->name);
    delete this->task_ptr;
    this->task_ptr = NULL;
    printf("%s killed\n", this->name);
  }
  else {
    printf("%s kill failed: already dead\n", this->name);
  }
}

void _Task::kill_without_notify(){ // kills the task unsafely
  if(this->task_ptr != NULL){
    printf("%s killing", this->name);
    this->task_ptr->remove();
    printf("%s kill command sent", this->name);
    wait_until(this->task_ptr->get_state() == E_TASK_STATE_DELETED){
      printf("%s state %d", this->name, this->task_ptr->get_state());
    }
    printf("%s state check passed\n", this->name);
    delete this->task_ptr;
    this->task_ptr = NULL;
    printf("%s killed", this->name);
  }
  else {
    printf("%s kill failed: already dead\n", this->name);
  }
}

bool _Task::notify_handle(){
  switch((notify_types)this->task_ptr->notify_take(1,0)){
    case stop:
      return true;
      break;
    case reset:
      printf("%s paused", this->name);
      this->task_ptr->suspend();
      break;
  }
  return false;
}

bool _Task::suspend(){
  if(this->task_ptr == NULL ||this->task_ptr->get_state() >=3)return false;
  printf("%s pausing for data", this->name);
  this->task_ptr->notify_ext((int)reset, E_NOTIFY_ACTION_OWRITE,NULL);
  wait_until (this->task_ptr->get_state() == E_TASK_STATE_SUSPENDED);
  return true;
}

bool _Task::resume(){
  if(this->task_ptr == NULL || this->task_ptr->get_state() !=3)return false;
  this->task_ptr->resume();
  printf("%s done data update, resuming", this->name);
  return true;
}

void _Task::rebind(pros::task_fn_t function, void* params){
  printf("%s rebinding", this->name);
  this->kill();
  this->function = function;
  this->start(params);
  printf("%s rebound", this->name);
}

void _Task::rebind_without_notify(pros::task_fn_t function, void* params){ // rebinds the task unsafely
  printf("%s rebinding", this->name);
  this->kill_without_notify();
  this->function = function;
  this->start(params);
  printf("%s rebound", this->name);
}