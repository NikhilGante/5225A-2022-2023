#pragma once
#include <tuple>
#include "task.hpp"
#include "geometry.hpp"
#include "functional"

// should be called at beginning of every async function to free memory
#define HANDLE_ARGS(type)\
  type* args_ptr = (LiftMoveToTargetParams*)_Task::get_params(params);\
	type args = *args_ptr;\
  delete args_ptr;\
  args_ptr = nullptr;\

struct EmptyType{}; // default type that async library accepts

template <typename ParamType = EmptyType>
class AsyncObj{
  const char* name;
  pros::task_fn_t function;
  _Task& task;
  tuple<_Task*, ParamType*> args; // where the latest args to be passed into its function are stored

public:
  AsyncObj(const char* name, pros::task_fn_t function, _Task& task):
    name(name), function(function), task(task)
  {
    
  }

  void sync(ParamType params){
    args = std::make_tuple(&task, new ParamType(params)); // allocates params on the heap to be passed in to the function
    task.kill_without_notify();  // ensures task isn't running anymore 
    function(&args); 
  }

  void async(ParamType params){
    task.rebind_without_notify(function, new ParamType(params));
  }
};
