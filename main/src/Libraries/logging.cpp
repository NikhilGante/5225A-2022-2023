#include "logging.hpp"
#include "timer.hpp"
#include "task.hpp"
#include "gui.hpp"

#include <fstream>

std::string Logging::master_file_name{"/usd/data.txt"};
_Task Logging::task{"Logging"};
std::vector<Logging*> Logging::logs{};
Queue<char, 20000> Logging::master_queue{"Logging"};

Logging auton_log       {"auton"};
Logging tracking_data   {"tracking"};
Logging state_log       {"states"    , log_locations::both};
Logging sensor_data     {"sensor"    , log_locations::sd};
Logging misc            {"misc"      , log_locations::none};
Logging term            {"terminal"  , log_locations::t};
Logging log_d           {"log"       , log_locations::sd};
Logging task_log        {"tasks"     , log_locations::both, term_colours::ERROR, true};
Logging controller_queue{"controller", log_locations::none, term_colours::NONE, true};
Logging error           {"error"     , log_locations::both, term_colours::ERROR};

Logging::Logging(std::string name, log_locations log_location, term_colours print_colour, bool newline):
Counter{name}, file_name{name + ".txt"}, log_location{log_location}, newline{newline}, print_colour{print_colour}, id{sprintf2("$%02d", getID())} {
  logs.push_back(this);
}

void Logging::init(){
  using std::ofstream;
  bool file_openable = true;
  if(!usd::is_installed()){
    file_openable = false;
    alert::start("Logging can't work: No SD Card");
  }
  else{
    ofstream file{master_file_name, ofstream::trunc};
    if(!file.is_open()){
      file_openable = false;
      alert::start("Log File not found");
    }
  }

  if(!file_openable){
    for(Logging* log: getList()){
      log_locations& loc = log->log_location;
      if(loc == log_locations::sd || loc == log_locations::both) loc = log_locations::t;
    }
  }
  else{ //Logging to SD is all good
    {
      ofstream meta_data("/usd/meta_data.txt", ofstream::trunc | ofstream::out);
      for(Logging* log: getList()){
        if((log->log_location == log_locations::sd || log->log_location == log_locations::both)) meta_data << log->file_name << ',' << log->id << ',';
      }
    }

    task.start([](){
      Timer timer{"Logging Queue"};
      while(true){

        if(timer.getTime() > print_max_time){
          ofstream data{master_file_name, ofstream::app};
          master_queue.output(data);

          for(Logging* log: getList()){
            ofstream data{"/usd/" + log->file_name, ofstream::app};
            log->queue.output(data);
          }

          timer.reset();
        }

        if(!master_queue.empty() && master_queue.size() > print_max_size){
          ofstream data{master_file_name, ofstream::app};
          master_queue.output(data);
          timer.reset();
        }

        for(Logging* log: getList()){
          if(!log->queue.empty() && log->queue.size() > print_max_size){
            ofstream data{"/usd/" + log->file_name, ofstream::app};
            log->queue.output(data);
            timer.reset();
          }
        }

        _Task::delay(10);
      }
    });
  }
}