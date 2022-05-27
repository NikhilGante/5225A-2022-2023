#include <tuple>
#include "task.hpp"
#include "geometry.hpp"
#include "functional"

template <typename ParamType>
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
    task.kill();  // ensures task isn't running anymore 
    function(&args); 
  }
  void async(ParamType params){
    task.rebind(function, new ParamType(params));
  }
};
