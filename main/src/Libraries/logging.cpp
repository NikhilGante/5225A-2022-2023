#include "logging.hpp"
#include "timer.hpp"
#include "task.hpp"
#include "gui.hpp"
#include "../config.hpp"

#include <fstream>

extern Page logging;

_Task Logging::task{"Logging"};
std::vector<Logging*> Logging::logs{};

Logging master_log    {"Master", false, term_colours::NONE, log_locations::sd};
Logging tracking_log  {"Tracking"};
Logging state_log     {"States"    , true};
Logging auton_log     {"Auton"     , true};
Logging shoot_log     {"Shooter"   , true};
Logging intake_log    {"Intake"    , true};
Logging controller_log{"Controller", true , term_colours::NONE, log_locations::sd};
Logging task_log      {"Tasks"     , true , term_colours::ERROR};
Logging error         {"Error"     , false, term_colours::ERROR};
Logging misc          {"Misc"      , false, term_colours::NONE, log_locations::none};
Logging driver_log    {"Driver"    , false, term_colours::NONE, log_locations::terminal};
Logging term          {"Terminal"  , false, term_colours::NONE, log_locations::terminal};
Logging sensor_log    {"Sensor"    , false, term_colours::NONE, log_locations::sd};
Logging log_d         {"Log"       , false, term_colours::NONE, log_locations::sd};

Logging::Logging(std::string name, bool newline, term_colours print_colour, log_locations location):
name{name}, newline{newline}, print_colour{print_colour}, location{location} {
  logs.push_back(this); //! Fix the issue with Counter

  //4x5
  //(20, 110, 200, 290, 380) x (15, 65, 115, 165)
  print_btn.construct(90*(getID()%5) + 20, 50*std::floor(getID()/5) + 40, 80, 40, GUI::Style::SIZE, Button::SINGLE, &logging, name, Color::dark_orange, Color::black);

  print_btn.setFunc([name](){
    printf2("\n\n%s Log Terminal Dump", name);
    std::ifstream file{"/usd/" + name + ".txt", std::ofstream::app};
    if(file.is_open()) std::cout << file.rdbuf() << "\n\n" << std::endl;
    else alert::start("Could not open %s log file", name);
  });
}

void Logging::init(){
  if(!usd::is_installed()){
    alert::start("Logging Inactive");
    for(Logging* log: getList()){
      log_locations& loc = log->location;
      if(loc == log_locations::sd || loc == log_locations::both) loc = log_locations::terminal;
      log->print_btn.setActive(false);
    }
  }
  else task.start([](){ //Logging is good to go
    Timer timer{"Logging Queue"};
    while(true){
      for(Logging* log: getList()){
        if(!log->queue.empty() && (log->queue.size() > print_max_size || timer.getTime() > print_max_time)){
          std::ofstream data{"/usd/" + log->name + ".txt", std::ofstream::app};
          log->queue.output(data);
          timer.reset();
        }
      }

      _Task::delay(10);
    }
  });
}