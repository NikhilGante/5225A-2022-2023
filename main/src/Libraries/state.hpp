#pragma once
#include <iostream>
#include <unordered_map>
#include <exception>
#include <variant>
#include <atomic>
#include "main.h"

#include "task.hpp"
#include "../util.hpp"

//! Why does every state have getName()? That is not a good way to do it

// class TaskEndException: public std::exception{
// public:
//   const char* what();
// };

// struct aParams{
//   int speed;
//   bool brake = true;
//   const char* getName(){
//     return "";
//   }
//   void handle(){

//   }
//   void handleStateChange(size_t prev_state){

//   }
// };

// struct bParams{
//   const char* getName(){
//     return "";
//   }
//   void handle(){

//   }
//   void handleStateChange(size_t prev_state){
//     // if(prev_state == GET_ID(aParams)) 
//   }
// };

template <typename... StateTypes>
class Machine{
  using variant = std::variant<StateTypes...>;
  variant state, target_state;
  pros::Mutex state_mutex, target_state_mutex;
  const char* name;

  std::atomic<bool> state_change_requested = false;

  _Task task;

  // Getters and setters for state and target state (since they need mutexes)
  void setState(variant state_param){
    state_mutex.take(TIMEOUT_MAX);
    state = state_param;
    state_mutex.give();
  }

  void setTargetState(variant target_state_param){
    target_state_mutex.take(TIMEOUT_MAX);
    target_state = target_state_param;
    target_state_mutex.give();
  }

public:
  template <typename base_state_type>
  Machine(const char* name, base_state_type base_state):  name(name), state(base_state), target_state(base_state){}

  template <typename next_state_type>
  void changeState(next_state_type next_state){
    printf("%s state change requested from %s to %s\n", name, getStateName(state), getStateName(next_state));
    setTargetState(next_state);
    state_change_requested = true;
    task.kill();  // Interrupts current state
  }

  // Getters for state and target state (since they need mutexes)

  variant getState(){
    state_mutex.take(TIMEOUT_MAX);
    variant temp = state;
    state_mutex.give();
    return temp;
  }

  variant getTargetState(){
    target_state_mutex.take(TIMEOUT_MAX);
    variant temp = target_state;
    target_state_mutex.give();
    return temp;
  }


  void runMachine(){
    task.start([&](){
      while(true){
        try{
          visit([](auto&& arg){arg.handle();}, state);  // Calls handler for current state
        }
        catch(const TaskEndException& exception){
        }

        if(state_change_requested){
          variant target_state_cpy = getTargetState();
          variant state_cpy = getState();
          // calls handle state change method for target state and logs the change
          state_log.print("%s state change started from %s to %s\n", name, getStateName(state_cpy), getStateName(target_state_cpy));
          visit([&](auto&& arg){arg.handleStateChange(state_cpy);}, target_state_cpy);
          state_log.print("%s state change finished from %s to %s\n", name, getStateName(state_cpy), getStateName(target_state_cpy));
          setState(target_state_cpy);

          state_change_requested = false;
        }
        delay(10);
      }
    });
  }

  const char* getStateName(variant state){
    return visit([](auto&& arg){return arg.getName();}, state);
  }

  void waitToReachState(variant state_param){  // blocks until desired state is reached
    size_t index = state_param.index();
    WAIT_UNTIL(getTargetState().index() == index && getState().index() == index);
  }

};
