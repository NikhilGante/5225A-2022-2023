#pragma once
#include <iostream>
#include <unordered_map>
#include <exception>
#include "task.hpp"
#include "main.h"

template<typename state_type>
class Subsystem{
  state_type state, target_state;
  pros::Mutex state_mutex, target_state_mutex;
  const char* name;
  std::unordered_map<state_type, const char*> state_to_state_name;

  static void taskStartFn(void* params){
    ((Subsystem<state_type>*)params)->run_machine();  // calls run machine from this
  };
  _Task task{taskStartFn, name};

  void run_machine(){
    while(true){
      try{
        handle();
      }
      catch(const TaskEndException& exception){
      }

      if(target_state != state){
        printf("%s state change started from %s to %s\n", name, get_state_name(state), get_state_name(target_state));
        handle_state_change(target_state);
        printf("%s state change finished from %s to %s\n", name, get_state_name(state), get_state_name(target_state));
        target_state = state;
      }
      delay(10);
    }
  }

  const char* get_state_name(state_type state){
    try {
      return state_to_state_name.at(state);
    }
    catch(const std::out_of_range& exception){
      return "Unknown state";
    }
  }

public:
  Subsystem(const char* name, state_type base_state, const std::unordered_map<state_type, const char*>& state_to_state_name):  name(name), state(base_state), state_to_state_name(state_to_state_name){

  }

  void change_state(state_type next_state){
    printf("%s state change requested from %s to %s\n", name, get_state_name(state), get_state_name(next_state));
    state_mutex.take(TIMEOUT_MAX);
    target_state = next_state;
    state_mutex.give();
  }

  state_type get_state(){
    state_mutex.take(TIMEOUT_MAX);
    state_type temp = state;
    state_mutex.give();
    return temp;
  }

  state_type get_target_state(){
    target_state_mutex.take(TIMEOUT_MAX);
    state_type temp = target_state;
    target_state_mutex.give();
    return temp;
  }

  void start_task(){
    task.start((void*)this);
  }


protected:

  virtual void handle_state_change(state_type state) = 0;
  virtual void handle() = 0;


};
