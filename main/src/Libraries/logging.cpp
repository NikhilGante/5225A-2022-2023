#include "logging.hpp"
#include "timer.hpp"
#include "task.hpp"
#include "gui.hpp"

#include <fstream>

extern Page logging;

Button Logging::master_print_btn(20, 15, 80, 40, GUI::Style::SIZE, Button::SINGLE, logging, "Master Terminal Dump");
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

Logging::Logging(std::string name, bool newline, term_colours print_colour, log_locations location):
name{name}, newline{newline}, print_colour{print_colour}, location{location} {
  logs.push_back(this); //! Fix the issue with Counter

  //4x5
  //(20, 110, 200, 290, 380) x (15, 65, 115, 165)
  //insert one more for master file
  print_btn.construct(90*(getID()%5) + 20, 50*std::floor(getID()/4) + 15, 80, 40, GUI::Style::SIZE, Button::SINGLE, &logging, name, Color::dark_orange, Color::black);

  print_btn.setFunc([name](){
    std::cout << "\n\n" << name << " Log Terminal Dump";
    std::ifstream file{"/usd/" + name + ".txt", std::ofstream::app};
    if(file.is_open()) std::cout << file.rdbuf() << "\n\n" << std::endl;
    else std::cout << "Could not open" + name + " log file" << std::endl;
  });
}

void Logging::init(){
  master_print_btn.setFunc([](){
    std::cout << "\n\nMaster Log Terminal Dump";
    std::ifstream file{master_file_name};
    if(file.is_open()) std::cout << file.rdbuf() << "\n\n" << std::endl;
    else std::cout << "Could not open Master log file" << std::endl;
  });

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
      log_locations& loc = log->location;
      if(loc == log_locations::sd || loc == log_locations::both) loc = log_locations::terminal;
      log->print_btn.setActive(false);
    }
    master_print_btn.setActive(false);
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