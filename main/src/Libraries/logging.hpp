#pragma once
#include "../util.hpp"
#include "printing.hpp"
#include "task.hpp"
#include "queue.hpp"
#include "../config.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <cstdarg>
#include <fstream>
#include <functional>


using namespace std;
using namespace pros;
//forawrd declarations
class _Task_;
class Timer;
class Data;
extern _Task_ log_t;

// extern Mutex log_m;

/*
1 is print, if there is no sd card, print to terminal
2 is print. if there is no sd card, ignore
0 is off
*/
enum log_types{
  error = 1,
  warning =1,
  general =1,
  debug = 2,
  off = 0,
};

enum class log_locations{
  t,
  sd,
  both,
  none
};

extern Data task_log;
extern Data controller_queue;
extern Data tracking_data;
extern Data tracking_imp;
extern Data misc;
extern Data drivers_data;
extern Data term;
extern Data motion_d;
extern Data motion_i;
extern Data log_d;
extern Data graph;
extern Data state_log;
extern Data graph;
extern Data skills_d;
extern Data safety;
extern Data ERROR;

// inline void log_init(){
//   std::ofstream file("/usd/data.txt", ofstream::trunc);
// }

// template <typename... Params>
// void log(term_colours colour, std::string format, log_locations log_location, Params... args){
//   std::string str = sprintf2(format, args...);

//   log_m.take();
//   switch(log_location){
//     case log_locations::none:
//       break;
//     case log_locations::t:
//       printf("%s", sprintf2_colour(colour, str).c_str());
//       break;
//     case log_locations::both:
//       printf("%s", sprintf2_colour(colour, str).c_str());
//     case log_locations::sd: //fallthrough intentional
//       std::ofstream file("/usd/data.txt", ofstream::app);
//       file.write(str.c_str(), str.length());
//       break;
//   }
//   log_m.give();
// }

// template <typename... Params>
// void log(std::string format, log_locations log_location, Params... args){
//   std::string str = sprintf2(format, args...);

//   log_m.take();
//   switch(log_location){
//     case log_locations::none:
//       break;
//     case log_locations::t:
//       printf("%s", str.c_str());
//       break;
//     case log_locations::both:
//       printf("%s", str.c_str());
//     case log_locations::sd: //fallthrough intentional
//       std::ofstream file("/usd/data.txt", ofstream::app);
//       file.write(str.c_str(), str.length());
//       break;
//   }
//   log_m.give();
// }

// template <typename... Params>
// void log(std::string format, Params... args){
//   log(format, log_locations::both, args...);
// }

// template <typename... Params>
// void log(term_colours colour, std::string format, Params... args){
//   log(colour, format, log_locations::both, args...);
// }

class Data{
private:
  static vector<Data*> obj_list;
  term_colours print_colour;
  bool newline;
  
public:
  static Queue<char, 1024> queue;
  static _Task_ log_t;

  std::string id, name;
  log_types log_type;
  log_locations log_location;
  // static vector<Data*> get_objs();

  // void print(Timer* tmr,int freq, vector<function<char*()>> str) const; //How is this used?
  void log_print(char* buffer, int buffer_len) const;
  // void set_print(term_colours print_colour, int time_type);

  Data(std::string obj_name, std::string id_code, log_types log_type_param, log_locations log_location_param = log_locations::both, bool newline = true, term_colours print_colour = term_colours::NONE);
  
  static void init();
  // static char* to_char(const char* format, ...);


  template <typename... Params>
  void print(term_colours colour, std::string format, Params... args) const{
    char newline = this->newline ? '\n' : '\0';
    std::string str = sprintf2(format, args...) + newline;

    // int buffer_len = str.length() + 3;
    // char buffer[256];
    // int buffer_len = snprintf(buffer, 256, str.c_str()) + 3;

    if(this->log_type != log_types::off){
      switch(log_location){
        case log_locations::t:
          // printf("%s", sprintf2_colour(colour, str).c_str());
          break;
        case log_locations::both:
          // printf("%s", sprintf2_colour(colour, str).c_str());
        case log_locations::sd: //fallthrough intentional
          // this->log_print(buffer, buffer_len);
          queue.insert(str.c_str());
          break;
        case log_locations::none:
          break;
      }
    }
  }

  template <typename... Params>
  void print(std::string format, Params... args) const{
    print(print_colour, format, args...);
  }
};


void queue_handle();
// uintptr_t data_size();
// constexpr int queue_size = 1024;
// constexpr int print_point = 500;
// constexpr int print_max_time = 500;
constexpr size_t print_point = 500;
constexpr uint32_t print_max_time = 500;