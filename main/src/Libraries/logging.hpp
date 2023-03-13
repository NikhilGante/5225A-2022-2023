#pragma once
#include "gui.hpp"
#include "../util.hpp"

enum class log_locations{
  none,
  terminal,
  both,
  sd_main,
  sd_only,
};

class Logging;
class _Task;

extern Logging tracking_log;
extern Logging drive_log;
extern Logging flywheel_log;
extern Logging state_log;
extern Logging subsystem_log;
extern Logging system_log;
extern Logging none_log;
extern Logging important_log;

class Logging: public ObjectTracker<Logging>{
  private:
    Button print_btn;
    term_colours print_colour;
    bool newline;
    log_locations location;
    std::string fullName, pastFullName;
    Queue<char, 20000> queue;
    Mutex queue_mutex;

    static constexpr std::size_t print_max_size{12000};
    static constexpr uint32_t print_max_time{800};
    static _Task task;
    static Button past_logs;
    static void pause();
    static void resume();
    static bool active;

    void update(bool force = false);

  public:
    Logging(std::string name, bool newline = false, log_locations location = log_locations::sd_main, term_colours print_colour = term_colours::NONE);

    static void init();
    
    void operator() (bool important, term_colours colour, std::string format, auto... args){
      if(!active) return;
      if(location == log_locations::none) return;

      std::string str{sprintf2(format, args...)};
      if(newline) str += '\n';

      if(important && this != &important_log) {important_log(colour, str);}
      if(location == log_locations::terminal || location == log_locations::both) printf2(colour, str);
      if(location == log_locations::sd_main || location == log_locations::sd_only || location == log_locations::both){
        queue_mutex.take(TIMEOUT_MAX);
        queue.insert(str);
        queue_mutex.give();
      }
    }

    void operator() (std::string format, auto... args) {(*this)(false, print_colour, format, args...);}
    void operator() (const char* format, auto... args) {(*this)(false, print_colour, format, args...);} //!Need this overload because bool is better implicit conversion for const char* than std::string
    void operator() (term_colours colour, std::string format, auto... args) {(*this)(false, colour, format, args...);}
    void operator() (bool important, std::string format, auto... args) {(*this)(important, print_colour, format, args...);}


    template <std::derived_from<std::ios> T>
    class Interrupter{
      public:
        T stream;
        Interrupter(std::string filename, std::ios_base::openmode mode) {
          system_log(term_colours::NOTIF, "%d: Interrupting Logging for %s", millis(), filename);
          pause();
          stream.open(filename, mode);
          if(!stream.is_open()) alert::start("Unable to open %s file when Interrupting Logging", filename);
        }
        ~Interrupter(){
          stream.close();
          resume();
          system_log(term_colours::NOTIF, "%d: Ended Logging Interruption", millis());
        }
    };
};