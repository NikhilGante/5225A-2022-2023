#include "logging.hpp"
#include "timer.hpp"
#include "task.hpp"
#include "gui.hpp"

#include <fstream>

std::string Logging::master_file_name{"/usd/MasterLog.txt"};
_Task Logging::task{"Logging"};
std::vector<Logging*> Logging::logs{};
Queue<char, 20000> Logging::master_queue{"Logging"};

Logging tracking_log  {"Tracking"};
Logging state_log     {"States"    , true};
Logging auton_log     {"Auton"     , true};
Logging shoot_log     {"Shooter"   , true};
Logging intake_log    {"Intake"    , true};
Logging controller_log{"Controller", true , term_colours::NONE, log_locations::sd};
Logging task_log      {"Tasks"     , true , term_colours::ERROR};
Logging error         {"Error"     , false, term_colours::ERROR};
Logging misc          {"Misc"      , false, term_colours::NONE, log_locations::terminal};
Logging driver_log    {"Driver"    , false, term_colours::NONE, log_locations::terminal};
Logging term          {"Terminal"  , false, term_colours::NONE, log_locations::terminal};
Logging sensor_log    {"Sensor"    , false, term_colours::NONE, log_locations::sd};
Logging log_d         {"Log"       , false, term_colours::NONE, log_locations::sd};

Logging::Logging(std::string name, bool newline, term_colours print_colour, log_locations log_location):
name{name}, newline{newline}, print_colour{print_colour}, log_location{log_location} {logs.push_back(this);} //! Fix the issue with Counter

void Logging::init(){
  using std::ofstream;
  bool file_openable = true;
  if(!usd::is_installed()){
    file_openable = false;
    alert::start("No SD Card");
  }
  else{
    ofstream file{master_file_name, ofstream::trunc};
    if(!file.is_open()){
      file_openable = false;
      alert::start("Master Log File not found");
    }
  }

  if(!file_openable){
    alert::start("Logging Deactivating");
    for(Logging* log: getList()){
      log_locations& loc = log->log_location;
      if(loc == log_locations::sd || loc == log_locations::both) loc = log_locations::terminal;
    }
  }
  else task.start([](){ //Logging is good to go
    Timer timer{"Logging Queue"};
    while(true){

      if(!master_queue.empty() && (master_queue.size() > print_max_size || timer.getTime() > print_max_time)){
        ofstream data{master_file_name, ofstream::app};
        master_queue.output(data);
        timer.reset();
      }

      for(Logging* log: getList()){
        if(!log->queue.empty() && (log->queue.size() > print_max_size || timer.getTime() > print_max_time)){
          ofstream data{"/usd/" + log->name + ".txt", ofstream::app};
          log->queue.output(data);
          timer.reset();
        }
      }

      _Task::delay(10);
    }
  });
}