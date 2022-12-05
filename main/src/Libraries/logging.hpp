#pragma once
#include "../util.hpp"
#include "queue.hpp"

enum class log_locations{
  t,
  sd,
  both,
  none
};

//All dump into the same queue
class Data{
  private:
    term_colours print_colour;
    bool newline;
    std::string name;
    log_locations log_location;

    static constexpr size_t print_max_size = 10000;
    static constexpr uint32_t print_max_time = 500;
    static std::string file_name;
    static std::vector<Data*> obj_list;
    static Queue<char, 20000> queue;
    static _Task_ task;
    static Mutex mutex;

  public:
    Data(std::string name, log_locations log_location = log_locations::both, term_colours print_colour = term_colours::NONE, bool newline = false);
    
    static void init();

    template <typename... Params>
    void print(term_colours colour, std::string format, Params... args) const{
      char newline = this->newline ? '\n' : '\0';
      std::string str = sprintf2(format, args...) + newline;

      switch(log_location){
        case log_locations::both:
          printf2(colour, str);
        case log_locations::sd: //fallthrough intentional
          mutex.take();
          queue.insert(str.cbegin(), str.cend());
          mutex.give();
          break;
        case log_locations::t:
          printf2(colour, str);
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
extern Data state_log;
extern Data tracking_data;
extern Data controller_queue;
extern Data misc;
extern Data term;
extern Data log_d;
extern Data error;