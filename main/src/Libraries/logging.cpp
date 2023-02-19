#include "logging.hpp"
#include "timer.hpp"
#include "gui.hpp"
#include "task.hpp"
#include "../config.hpp"

#include <fstream>
#include <filesystem>

extern Page logging;
extern Button clear_logs;

std::string Logging::folder_name{"Empty Log Folder Name"};
_Task Logging::task{"Logging"};
std::vector<Logging*> Logging::logs{};

Logging master_log    {"Master"    , false, term_colours::NONE, log_locations::sd_only};
Logging state_log     {"States"    , true};
Logging auton_log     {"Auton"     , true};
Logging task_log      {"Tasks"     , true , term_colours::ERROR};
Logging error_log     {"Error"     , false, term_colours::ERROR};
Logging tracking_log  {"Tracking"  , false, term_colours::NONE, log_locations::sd_main};
Logging controller_log{"Controller", true , term_colours::NONE, log_locations::sd_only};
Logging device_log    {"Device"    , false, term_colours::NONE, log_locations::sd_only};
Logging driver_log    {"Driver"    , false, term_colours::NONE, log_locations::sd_main};

Logging::Logging(std::string name, bool newline, term_colours print_colour, log_locations location):
name{name}, newline{newline}, print_colour{print_colour}, location{location} {
  logs.push_back(this); //! Fix the issue with ObjectTracker

  //4x5
  //(20, 110, 200, 290, 380) x (15, 65, 115, 165)
  print_btn.construct(90*(getID()%5) + 20, 50*std::floor(getID()/5) + 40, 80, 40, GUI::Style::SIZE, Button::SINGLE, &logging, name, Color::dark_orange, Color::black);

  print_btn.setFunc([this](){
    printf2("\n\n%s Log Terminal Dump", this->name);
    std::cout << Interrupter<std::ifstream>(fullName()).stream.rdbuf() << "\n\n" << std::endl;
  });
}

void Logging::init(){
  if(!usd::is_installed()){
    alert::start("Logging Inactive");
    clear_logs.setActive(false);
    for(Logging* log: getList()){
      switch(log->location){
        case log_locations::terminal:
        case log_locations::both:
        case log_locations::sd_main: log->location = log_locations::terminal; break;
        case log_locations::sd_only:
        case log_locations::none:    log->location = log_locations::none; break;
      }
    }
  }

  for(Logging* log: getList()){
    if(log->location == log_locations::terminal || log->location == log_locations::none) log->print_btn.setActive(false);
  }


  std::time_t current_time;
  std::time(&current_time);
  std::string date = asctime(localtime(&current_time));
  date = std::string(date.begin()+4, date.end()-6);
  folder_name = "/usd/Logging/" + std::string(date.begin()+4, date.end()-6) + '/';\
  std::filesystem::create_directories(folder_name);

  task.start([](){ //Logging is good to go
    Timer timer{"Logging Queue", task_log};
    while(true){
      for(Logging* log: getList()){        
        if(log->update(timer.getTime() > print_max_time)) timer.reset();
      }

      _Task::delay(10);
    }
  });
}

bool Logging::update(bool time_exceeded){
  if(!queue.empty() && (time_exceeded || queue.size() > print_max_size)){

    std::ofstream data{fullName(), std::ofstream::app};
    switch(location){
      case log_locations::sd_main:
      case log_locations::sd_only:  queue.output(data); break;
      case log_locations::terminal: queue.output(std::cout); break;
      case log_locations::both:     queue.output(data, std::cout); break;
      case log_locations::none:     alert::start("Log %s has a non-empty queue for none output", name); break;
    }
    return true;
  }
  return false;
}

std::string Logging::fullName() {return folder_name + name + ".txt";}
void Logging::pause() {task.suspend();}
void Logging::resume() {task.resume();}