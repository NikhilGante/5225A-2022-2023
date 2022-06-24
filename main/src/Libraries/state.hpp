#pragma once
#include <iostream>
#include <unordered_map>
#include <exception>
#include <variant>
#include <atomic>
#include "main.h"

#include "task2.hpp"

using namespace std;
using namespace pros;

#define GET_ID(type)\
  typeid(type).hash_code()

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
class Subsystem{
  variant<StateTypes...> state, target_state;
  pros::Mutex state_mutex, target_state_mutex;
  const char* name;

  atomic<size_t> state_id, target_state_id;


public:
  template <typename base_state_type>
  Subsystem(const char* name, base_state_type base_state):  name(name), state(base_state), state_id(typeid(base_state).hash_code()){

  }
  template <typename next_state_type>
  void change_state(next_state_type next_state){
    printf("%s state change requested from %s to %s\n", name, getStateName(state), getStateName(next_state));
    state_mutex.take(TIMEOUT_MAX);
    target_state = next_state;
    target_state_id = typeid(next_state).hash_code();
    state_mutex.give();
  }

  variant<StateTypes...> get_state(){
    state_mutex.take(TIMEOUT_MAX);
    variant<StateTypes...> temp = state;
    state_mutex.give();
    return temp;
  }

  size_t get_state_id(){
    return state_id;
  }

  variant<StateTypes...> get_target_state(){
    target_state_mutex.take(TIMEOUT_MAX);
    variant<StateTypes...> temp = target_state;
    target_state_mutex.give();
    return temp;
  }

  size_t get_target_state_id(){
    return target_state_id;
  }

  void run_machine(){
    while(true){
      try{
        visit([](auto&& arg){arg.handle();}, state);
      }
      catch(const TaskEndException& exception){
      }

      if(target_state.index() != state.index()){
        printf("%s state change started from %s to %s\n", name, getStateName(state), getStateName(target_state));
        visit([&](auto&& arg){arg.handleStateChange(state_id);}, target_state);
        printf("%s state change finished from %s to %s\n", name, getStateName(state), getStateName(target_state));
        state = target_state;
        state_id = target_state_id.load();

      }
      delay(10);
    }
  }

  const char* getStateName(variant<StateTypes...> state){
    return visit([](auto&& arg){return arg.getName();}, state);
  }

};
