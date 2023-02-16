#pragma once
#include "queue.hpp"
#include "printing.hpp"
#include "counter.hpp"
#include <fstream>

enum class log_locations{
  terminal,
  sd,
  both,
  none
};

class Logging;
class _Task;

extern Logging master_log    ;
extern Logging tracking_log  ;
extern Logging state_log     ;
extern Logging auton_log     ;
extern Logging controller_log;
extern Logging task_log      ;
extern Logging error         ;
extern Logging misc          ;
extern Logging driver_log    ;
extern Logging term          ;
extern Logging device_log    ;
extern Logging log_d         ;

class Logging: public Counter<Logging>{
  private:
    Button print_btn;
    term_colours print_colour;
    bool newline;
    log_locations location;
    std::string name;
    Queue<char, 5000> queue;

    static constexpr size_t print_max_size{10000};
    static constexpr uint32_t print_max_time{500};
    static _Task task;
    static std::vector<Logging*> logs;

    static void pause();
    static void restart();

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
          master_log.queue.insert(name + ": ");
          master_log.queue.insert(str);
          if(this != &master_log) queue.insert(str);
          break;
        case log_locations::terminal:
          printf2(colour, str);
          break;
        case log_locations::none:
          break;
      }
    }

    void operator() (std::string format, auto... args) {(*this)(print_colour, format, args...);}

    template <std::derived_from<std::ios> T>
    class Interrupter{
      public:
        T stream;
        Interrupter(std::string filename): stream{"/usd/" + filename + ".txt"} {
          Logging::pause();
          if(!stream.is_open()) alert::start("Unable to open %s file when Interrupting Logging", filename);
        }
        ~Interrupter() {Logging::restart();}
    };
};