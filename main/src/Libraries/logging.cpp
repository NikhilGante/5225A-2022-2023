#include "logging.hpp"
#include "timer.hpp"
#include "gui.hpp"
#include "task.hpp"
#include "../config.hpp"

#include <fstream>

extern Page logging;

_Task Logging::task{"Logging"};
std::vector<Logging*> Logging::logs{};

Logging master_log    {"Master", false, term_colours::NONE, log_locations::sd_only};
Logging tracking_log  {"Tracking"};
Logging state_log     {"States"    , true};
Logging auton_log     {"Auton"     , true};
Logging controller_log{"Controller", true , term_colours::NONE, log_locations::sd_only};
Logging task_log      {"Tasks"     , true , term_colours::ERROR};
Logging error         {"Error"     , false, term_colours::ERROR};
Logging misc          {"Misc"      , false, term_colours::NONE, log_locations::sd_only};
Logging driver_log    {"Driver"    , false, term_colours::NONE, log_locations::terminal};
Logging term          {"Terminal"  , false, term_colours::NONE, log_locations::terminal};
Logging device_log    {"Sensor"    , false, term_colours::NONE, log_locations::sd_only};

Logging::Logging(std::string name, bool newline, term_colours print_colour, log_locations location):
name{name}, newline{newline}, print_colour{print_colour}, location{location} {
  logs.push_back(this); //! Fix the issue with Counter

  //4x5
  //(20, 110, 200, 290, 380) x (15, 65, 115, 165)
  print_btn.construct(90*(getID()%5) + 20, 50*std::floor(getID()/5) + 40, 80, 40, GUI::Style::SIZE, Button::SINGLE, &logging, name, Color::dark_orange, Color::black);

  print_btn.setFunc([name](){
    printf2("\n\n%s Log Terminal Dump", name);
    std::cout << Interrupter<std::ifstream>(name).stream.rdbuf() << "\n\n" << std::endl;
  });
}

void Logging::init(){
  if(!usd::is_installed()){
    alert::start("Logging Inactive");
    for(Logging* log: getList()){
      log_locations& loc = log->location;
      if(loc == log_locations::sd_main || loc == log_locations::both) loc = log_locations::terminal;
      else if(loc == log_locations::sd_only) loc = log_locations::none;
    }
  }

  for(Logging* log: getList()){
    if(log->location == log_locations::terminal || log->location == log_locations::none) log->print_btn.setActive(false);
  }

  task.start([](){ //Logging is good to go
    Timer timer{"Logging Queue"};
    while(true){
      for(Logging* log: getList()){
        if(log->location == log_locations::sd_main && !log->queue.empty() && (log->queue.size() > print_max_size || timer.getTime() > print_max_time)){
          std::ofstream data{"/usd/" + log->name + ".txt", std::ofstream::app};
          log->queue.output(data);
          timer.reset();
        }
      }

      _Task::delay(10);
    }
  });
}

void Logging::pause() {task.suspend();}
void Logging::resume() {task.resume();}