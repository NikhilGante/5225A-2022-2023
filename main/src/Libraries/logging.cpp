#include "logging.hpp"
#include "timer.hpp"
#include "task.hpp"
#include "gui.hpp"

#include <fstream>

std::string Logging::file_name{"/usd/data.txt"};
_Task Logging::task{"Logging"};
Queue<char, 20000> Logging::queue{"Logging"};

Logging task_log("tasks", log_locations::none);
Logging state_log("states", log_locations::both);
Logging sensor_data("states", log_locations::sd);
Logging auton_log("states");
Logging controller_queue("controller", log_locations::none, term_colours::NONE, true);
Logging tracking_data("tracking");
Logging misc("misc", log_locations::both);
Logging term("terminal", log_locations::t);
Logging log_d("log", log_locations::sd);
Logging error("error", log_locations::both, term_colours::ERROR);

Logging::Logging(std::string name, log_locations log_location, term_colours print_colour, bool newline):
Counter{name}, name{name + ".txt"}, log_location{log_location}, newline{newline}, print_colour{print_colour}, id{sprintf2("$%01d", getID())} {}

void Logging::init(){
  using std::ofstream;
  bool file_openable = true;
  if(!usd::is_installed()){
    file_openable = false;
    alert::start("Logging can't work: No SD Card");
  }
  else{
    ofstream file{file_name, ofstream::trunc};
    if(!file.is_open()){
      file_openable = false;
      alert::start("Log File not found");
    }
  }

  if(!file_openable){
    for(Logging* obj: getList()){
      log_locations& loc = obj->log_location;
      if(loc == log_locations::sd || loc == log_locations::both) loc = log_locations::t;
    }
  }
  else task.start([](){
    Timer timer{"Logging Queue"};

    while(true){
      if(!queue.empty() && (timer.getTime() > print_max_time || queue.size() > print_max_size)){
        ofstream file{file_name, ofstream::app};
        queue.output(file);
        timer.reset();
      }
      task_log.print("Nothing to Log\n");

      _Task::delay(10);
    }
  });
}