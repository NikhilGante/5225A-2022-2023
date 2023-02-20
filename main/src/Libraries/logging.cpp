#include "logging.hpp"
#include "timer.hpp"
#include "gui.hpp"
#include "task.hpp"
#include "../config.hpp"

#include <fstream>
#include <filesystem>

extern Page logging;
extern Button clear_logs;

_Task Logging::task{"Logging"};
std::vector<Logging*> Logging::logs{};

// Logging master_log    {"Master"    , false, term_colours::NONE, log_locations::none};
Logging state_log     {"States"    , true};
Logging auton_log     {"Auton"     , true , term_colours::NONE, log_locations::sd_main};
Logging task_log      {"Tasks"     , true , term_colours::ERROR, log_locations::sd_main};
Logging error_log     {"Error"     , false, term_colours::ERROR};
Logging tracking_log  {"Tracking"  , false, term_colours::NONE, log_locations::sd_main};
Logging controller_log{"Controller", true , term_colours::NONE, log_locations::sd_only};
Logging device_log    {"Device"    , true , term_colours::NONE, log_locations::sd_only};
Logging driver_log    {"Driver"    , false, term_colours::NONE, log_locations::sd_main};
Logging none_log      {"None"      , false, term_colours::NONE, log_locations::none};

Logging::Logging(std::string name, bool newline, term_colours print_colour, log_locations location):
name{name}, newline{newline}, print_colour{print_colour}, location{location}, queue{name} {
  logs.push_back(this); //! Fix the issue with ObjectTracker

  static int x = 110, y = 40;
  print_btn.construct(x, y, 80, 40, GUI::Style::SIZE, Button::SINGLE, &logging, name, Color::dark_orange, Color::black);
  x = x != 380 ? x+90 : 20;
  if ((getID()+2) % 5 == 0) y += 50;

  print_btn.setFunc([this](){
    printf2(term_colours::GREEN, "\n\nStart %s Log Terminal Dump\n", this->name);
    std::cout << get_term_colour(term_colours::BLUE);
    std::cout << Interrupter<std::ifstream>(fullName()).stream.rdbuf() << std::endl;
    printf2(term_colours::RED, "\nEnd %s Log Terminal Dump\n\n", this->name);
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
    std::ofstream file_init{log->fullName(), std::ofstream::trunc};
    file_init << "Start of " + log->name + " log file\n\n";
  }

  task.start([](){ //Logging is good to go
    Timer timer{"Logging Queue", task_log};
    while(true){
      if(timer.getTime() > print_max_time){
        for(Logging* log: getList()) log->update(timer.getTime(), true);
        timer.reset();
      }
      else{
        for(Logging* log: getList()) log->update(timer.getTime(), false);
      }

      _Task::delay(10);
    }
  });
}

void Logging::update(uint64_t time, bool force){
  if(!queue.empty() && (force || queue.size() > print_max_size)){
    if(location == log_locations::sd_main || location == log_locations::sd_only || location == log_locations::both){
      std::ofstream data{fullName(), std::ofstream::app};
      queue_mutex.take();
      // printf2("%s: F%d, TE%d SE%d T%d S%d\n", name, force, time > print_max_time, queue.size() > print_max_size, time, queue.size());
      queue.output(data);
      queue.clear();
      queue_mutex.give();
    }
    else queue.clear();
  }
}

std::string Logging::fullName() {return folder_name + name + ".txt";}
void Logging::pause() {task.suspend();}
void Logging::resume() {task.resume();}