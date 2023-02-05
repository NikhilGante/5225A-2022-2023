#pragma once
#include "queue.hpp"
#include "printing.hpp"
#include "counter.hpp"

class _Task;

enum class log_locations{
  terminal,
  sd,
  both,
  none
};

class Logging: public Counter<Logging>{
  private:
    static Button master_print_btn;
    Button print_btn;

    term_colours print_colour;
    bool newline;
    log_locations location;
    std::string name;
    Queue<char, 2000> queue;

    static constexpr size_t print_max_size{10000};
    static constexpr uint32_t print_max_time{500};
    static _Task task;
    static std::vector<Logging*> logs;
    static std::string master_file_name;
    static Queue<char, 20000> master_queue;

  public:
    Logging(std::string name, bool newline = false, term_colours print_colour = term_colours::NONE, log_locations location = log_locations::both);
    
    static void init();

    static std::vector<Logging*> const & getList() {return logs;}

    void operator() (term_colours colour, std::string format, auto... args){
      std::string str{sprintf2(format, args...)};
      if(newline) str += '\n';

      switch(location){
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

    void operator() (std::string format, auto... args) {(*this)(print_colour, format, args...);}
};

extern Logging tracking_log  ;
extern Logging state_log     ;
extern Logging auton_log     ;
extern Logging shoot_log     ;
extern Logging intake_log    ;
extern Logging controller_log;
extern Logging task_log      ;
extern Logging error         ;
extern Logging misc          ;
extern Logging driver_log    ;
extern Logging term          ;
extern Logging sensor_log    ;
extern Logging log_d         ;