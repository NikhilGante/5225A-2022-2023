#include "logging.hpp"
#include "timer.hpp"
#include "task.hpp"
#include <fstream>

std::string Logging::file_name{"/usd/data.txt"};
_Task Logging::task{"logging"};
Queue<char, 20000> Logging::queue{"Logging"};
std::vector<Logging*> Logging::obj_list;
Mutex Logging::mutex;

Logging task_log("tasks");
Logging state_log("states", log_locations::sd);
Logging auton_log("states");
Logging controller_queue("controller", log_locations::t, term_colours::NONE, true);
Logging tracking_data("tracking");
Logging misc("misc", log_locations::both);
Logging term("terminal", log_locations::t);
Logging log_d("log", log_locations::sd);
Logging error("error", log_locations::both, term_colours::ERROR);

Logging::Logging(std::string name, log_locations log_location, term_colours print_colour, bool newline):
name{name + ".txt"}, log_location{log_location}, newline{newline}, print_colour{print_colour} {obj_list.push_back(this);}

void Logging::init(){
  using std::ofstream;
  {
    ofstream file{file_name, ofstream::trunc};
    if(!file.is_open()){
      printf2(term_colours::ERROR, "Log File not found");
      for(Logging* obj: obj_list){
        log_locations& loc = obj->log_location;
        if(loc == log_locations::sd || loc == log_locations::both) loc = log_locations::t;
      }
    }
  } //To limit scope of ofstream file

  task.start([](){
    Timer timer{"logging_tmr"};

    while(true){
      if(!queue.empty() && (timer.getTime() > print_max_time || queue.size() > print_max_size)){
        mutex.take();
        ofstream file{file_name, ofstream::app};
        mutex.take();
        queue.output(file);
        mutex.give();

        timer.reset();
      }

      _Task::delay(10);
    }
  });
}