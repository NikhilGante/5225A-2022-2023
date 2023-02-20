#include "logging.hpp"
#include "printing.hpp"
#include "timer.hpp"
#include "gui.hpp"
#include "task.hpp"
#include "../config.hpp"
#include "../util.hpp"
#include "../Devices/controller.hpp"

#include <fstream>
#include <filesystem>

extern Page logging;
extern Button clear_logs;

_Task Logging::task{"Logging"};

// Logging master_log    {"Master"    , false, term_colours::NONE, log_locations::none};
Logging state_log     {"States"    , true , log_locations::sd_main};
Logging auton_log     {"Auton"     , true , log_locations::sd_main};
Logging tracking_log  {"Tracking"  , false, log_locations::sd_main};
Logging driver_log    {"Driver"    , false, log_locations::sd_main};
Logging controller_log{"Controller", true , log_locations::sd_only};
Logging device_log    {"Device"    , true , log_locations::sd_only};
Logging task_log      {"Tasks"     , true , log_locations::sd_main, term_colours::ERROR};
Logging error_log     {"Error"     , false, log_locations::none   , term_colours::ERROR};
Logging log_log       {"Log"       , true};
Logging none_log      {"None"      , false, log_locations::none};

Logging::Logging(std::string name, bool newline, log_locations location, term_colours print_colour):
name{name}, newline{newline}, location{location}, print_colour{print_colour}, queue{name} {
  static int x = 110, y = 40;
  print_btn.construct({x, y, 80, 40, GUI::Style::SIZE}, Button::SINGLE, &logging, name, Color::dark_orange, Color::black);
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
  log_log(term_colours::GREEN, "%d: Initializing Logging", millis());
  if(!usd::is_installed()){
    alert::start("SD Logging Inactive");
    log_log(term_colours::ERROR, "No SD Card, deactivating Logging");
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
    log_log("%d: Opening %s log file ", millis(), log->name);
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
      queue_mutex.take(TIMEOUT_MAX);
      // printf2("%s: T:%d | S:%d\n", name, force, queue.size() > print_max_size);
      // printf2("%s: A:%d | TE%d SE%d T%d S%d\n", name, task.isAlive(), force, queue.size() > print_max_size, time, queue.size());
      std::ofstream data{fullName(), std::ofstream::app};
      DEBUG;
      queue.output(data);
      DEBUG;
      queue.clear();
      queue_mutex.give();
    }
    else queue.clear();
  }
}

std::string Logging::fullName() {return folder_name + name + ".txt";}
void Logging::pause() {log_log("%d: Pausing Logging", millis()); task.suspend();}
void Logging::resume() {log_log("%d: Resuming Logging", millis()); task.resume();}