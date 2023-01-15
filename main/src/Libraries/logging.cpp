#include "logging.hpp"
#include "timer.hpp"
#include "task.hpp"
#include "gui.hpp"

#include <fstream>

std::string Logging::master_file_name{"/usd/MasterLog.txt"};
_Task Logging::task{"Logging"};
std::vector<Logging*> Logging::logs{};
Queue<char, 20000> Logging::master_queue{"Logging"};

Logging auton_log       {"Auton"};
Logging tracking_data   {"Tracking"};
Logging state_log       {"States"    , log_locations::both};
Logging sensor_data     {"Sensor"    , log_locations::sd};
Logging misc            {"Misc"      , log_locations::none};
Logging term            {"Terminal"  , log_locations::t};
Logging log_d           {"Log"       , log_locations::sd};
Logging task_log        {"Tasks"     , log_locations::both, term_colours::ERROR, true};
Logging controller_queue{"Controller", log_locations::none, term_colours::NONE, true};
Logging error           {"Error"     , log_locations::both, term_colours::ERROR};

Logging::Logging(std::string name, log_locations log_location, term_colours print_colour, bool newline):
Counter{name}, name{name}, log_location{log_location}, print_colour{print_colour}, newline{newline} {
  logs.push_back(this);
}

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
      if(loc == log_locations::sd || loc == log_locations::both) loc = log_locations::t;
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