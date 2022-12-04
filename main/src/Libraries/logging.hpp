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

using namespace pros;
//forawrd declarations
// class _Task_;
// class Timer;
// class Data;
// extern _Task_ log_t;

/*
1 is print, if there is no sd card, print to terminal
2 is print. if there is no sd card, ignore
0 is off
*/

enum class log_locations{
  t,
  sd,
  both,
  none
};


class Data{
  private:
    static vector<Data*> obj_list;
    static std::string file_name;
    term_colours print_colour;
    bool newline;

    static constexpr size_t print_point = 500;
    static constexpr uint32_t print_max_time = 500;
    
  public:
    static Queue<char, 1024> queue;
    static _Task_ log_t;

    std::string name;
    log_locations log_location;

    Data(std::string name, log_locations log_location = log_locations::both, bool newline = true, term_colours print_colour = term_colours::NONE);
    
    static void init();
    static void queue_handle();

    template <typename... Params>
    void print(term_colours colour, std::string format, Params... args) const{
      char newline = this->newline ? '\n' : '\0';
      std::string str = sprintf2(format, args...) + newline;

      switch(log_location){
        case log_locations::t:
          printf("%s", sprintf2_colour(colour, str).c_str());
          break;
        case log_locations::both:
          printf("%s", sprintf2_colour(colour, str).c_str());
        case log_locations::sd: //fallthrough intentional
          queue.insert(str.begin(), str.end());
          break;
        case log_locations::none:
          break;
      }
    }

    template <typename... Params>
    void print(std::string format, Params... args) const{
      print(print_colour, format, args...);
    }
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