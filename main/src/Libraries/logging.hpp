#pragma once
#include "queue.hpp"
#include "printing.hpp"
#include "counter.hpp"
#include "../config.hpp"

class _Task;

enum class log_locations{
  terminal,
  sd,
  both,
  none
};

//All dump into the same queue
class Logging: public Counter<Logging>{
  private:
    term_colours print_colour;
    bool newline;
    log_locations log_location;
    std::string name;
    Queue<char, 2000> queue;

    static constexpr size_t print_max_size{10000};
    static constexpr uint32_t print_max_time{500};
    static _Task task;
    static std::vector<Logging*> logs;
    static std::string master_file_name;
    static Queue<char, 20000> master_queue;

  public:
    Logging(std::string name, log_locations log_location = log_locations::both, term_colours print_colour = term_colours::NONE, bool newline = false);
    
    static void init();

    static std::vector<Logging*> const & getList() {return logs;}

    void print(term_colours colour, std::string format, auto... args){
      std::string str{sprintf2(format, args...)};
      if(newline) str += '\n';

      switch(log_location){
        case log_locations::both:
          printf2(colour, str);
        case log_locations::sd: //fallthrough intentional
          master_queue.insert(name + ": ");
          master_queue.insert(str);
          queue.insert(str);
          break;
        case log_locations::terminal:
          printf2(colour, str);
          break;
        case log_locations::none:
          break;
      }
    }

    void print(std::string format, auto... args) {print(print_colour, format, args...);}
};

extern Logging task_log;
extern Logging state_log;
extern Logging sensor_data;
extern Logging auton_log;
extern Logging tracking_data;
extern Logging controller_data;
extern Logging misc;
extern Logging term;
extern Logging log_d;
extern Logging error;