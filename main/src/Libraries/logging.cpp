#include "logging.hpp"
#include "timer.hpp"
#include "task.hpp"

#include <fstream>
#include <ostream>

extern Page logging;

Button Logging::past_logs {{15, 40, 100, 40, GUI::Style::SIZE}, Button::TOGGLE, logging, "Past Logs", Color::red};
_Task Logging::task{"Logging"};

// Logging master_log    {"Master"    , false, term_colours::NONE, log_locations::none};
Logging state_log     {"States"    , true , log_locations::sd_main};
Logging auton_log     {"Auton"     , true , log_locations::sd_main};
Logging tracking_log  {"Tracking"  , false, log_locations::sd_main};
Logging driver_log    {"Driver"    , false, log_locations::sd_main};
Logging controller_log{"Controller", true , log_locations::sd_only};
Logging device_log    {"Device"    , true , log_locations::sd_only};
Logging task_log      {"Tasks"     , true , log_locations::sd_main, term_colours::ERROR};
Logging error_log     {"Error"     , false, log_locations::both   , term_colours::ERROR};
Logging log_log       {"Log"       , true};
Logging none_log      {"None"      , false, log_locations::none};

Logging::Logging(std::string name, bool newline, log_locations location, term_colours print_colour):
ObjectTracker{"Logging", name}, queue{name}, name{name}, newline{newline}, location{location}, print_colour{print_colour} {
  static int x = 130, y = 40;
  print_btn.construct({x, y, 100, 40, GUI::Style::SIZE}, Button::SINGLE, &logging, name, Color::dark_orange, Color::black);
  x = x != 360 ? x+115 : 15;
  if ((getID()+1) % 4 == 0) y += 50;

  print_btn.setFunc([this](){
    printf2(term_colours::GREEN, "\n\nStart %s Log Terminal Dump\n", this->name);
    std::cout << get_term_colour(term_colours::BLUE);
    auto file = Interrupter<std::ifstream>(past_logs.isOn() ? pastFullName : fullName);
    if(file.stream.is_open()) std::cout << file.stream.rdbuf() << std::endl;
    else std::cout << fullName << " unopenable" << std::endl;
    printf2(term_colours::RED, "\nEnd %s Log Terminal Dump\n\n", this->name);
  });
}

void Logging::init(){
  log_log(term_colours::GREEN, "%d: Initializing Logging", millis());
  if(!usd::is_installed()){ //Rerouting data to non-sd card
    alert::start("SD Logging Inactive");
    log_log(term_colours::ERROR, "No SD Card, deactivating Logging");
    past_logs.setActive(false);
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
  else{ //Setting up the count for having past log files
    int count;
    {
      std::ifstream log_count{"/usd/log_count.txt"};
      if (log_count.is_open()) log_count >> count;
      else count = 0;
    }
    {
      log_log("Log Version %d", count);
      std::ofstream log_count{"/usd/log_count.txt"};
      log_count << count + 1; //For the next round of logs
    }

    for(Logging* log: getList()){
      log->fullName = "/usd/Logging/" + std::to_string(count) + ' ' + log->name + ".txt";
      log->pastFullName = "/usd/Logging/" + std::to_string(count-1) + ' ' + log->name + ".txt";
      log_log("%d: Opening %s log file on SD", millis(), log->fullName);
      std::ofstream file_init{log->fullName, std::ofstream::trunc};
      file_init << "Start of " + log->name + " - " + std::to_string(count) + " log file\n\n";
    }
  }

  for(Logging* log: getList()){
    if(log->location == log_locations::terminal || log->location == log_locations::none) log->print_btn.setActive(false);
  }

  task.start([](){ //Logging is good to go
    Timer timer{"Logging Queue"};
    while(true){
      if(timer.getTime() > print_max_time){
        for(Logging* log: getList()) log->update(true);
        timer.reset();
      }
      else{
        for(Logging* log: getList()) log->update();
      }
      _Task::delay(10);
    }
  });
}

void Logging::update(bool force){
  if(!queue.empty() && (force || queue.size() > print_max_size)){
    if(location == log_locations::sd_main || location == log_locations::sd_only || location == log_locations::both){
      queue_mutex.take(TIMEOUT_MAX);
      std::ofstream data{fullName, std::ofstream::app};
      queue.output(data);
      queue.clear();
      queue_mutex.give();
    }
    else queue.clear();
  }
}

void Logging::pause() {log_log("%d: Pausing Logging", millis()); task.suspend();}
void Logging::resume() {log_log("%d: Resuming Logging", millis()); task.resume();}