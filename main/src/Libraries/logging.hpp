#pragma once
#include "queue.hpp"
#include "printing.hpp"
#include "tracker.hpp"
#include <fstream>

enum class log_locations{
  none,
  terminal,
  both,
  sd_main,
  sd_only,
};

class Logging;
class _Task;

extern Logging master_log    ;
extern Logging tracking_log  ;
extern Logging state_log     ;
extern Logging auton_log     ;
extern Logging controller_log;
extern Logging task_log      ;
extern Logging error_log     ;
extern Logging driver_log    ;
extern Logging device_log    ;
extern Logging none_log      ;

class Logging: public ObjectTracker<Logging>{
  private:
  public:
    Button print_btn;
    term_colours print_colour;
    bool newline;
    log_locations location;
    std::string name;
    Queue<char, 15000> queue;
    Mutex queue_mutex;

    static constexpr size_t print_max_size{10000};
    static constexpr uint32_t print_max_time{1000};
    inline static const std::string folder_name = "/usd/Logging/";
    static _Task task;
    static std::vector<Logging*> logs; //! Get rid of this after fixing ObjectTracker

    std::string fullName();
    void update(uint64_t time, bool force);

  public:
    Logging(std::string name, bool newline = false, term_colours print_colour = term_colours::NONE, log_locations location = log_locations::both);
    
    static void init();
    static std::vector<Logging*> const & getList() {return logs;}
    static void pause();
    static void resume();

    void operator() (term_colours colour, std::string format, auto... args){
      if (location == log_locations::none) return;

      std::string str{sprintf2(format, args...)};
      if(newline) str += '\n';

      if(location == log_locations::terminal || location == log_locations::both) printf2(colour, str);
      else if(location == log_locations::sd_main || location == log_locations::sd_only){
        queue_mutex.take();
        queue.insert(str);
        queue_mutex.give();
      }
    }

    void operator() (std::string format, auto... args) {(*this)(print_colour, format, args...);}

    template <std::derived_from<std::ios> T>
    class Interrupter{
      public:
        T stream;
        Interrupter(std::string filename) {
          pause();
          stream.open(filename);
          if(!stream.is_open()) alert::start("Unable to open %s file when Interrupting Logging", filename);
        }
        ~Interrupter() {stream.close(); resume();}
    };
};