#include "logging.hpp"
#include "alert.hpp"
#include "timer.hpp"
#include "task.hpp"

#include <fstream>

Page logging {"Logging"}; //Log printing page from file to terminal
Button Logging::past_logs {{15, 40, 100, 40, GUI::Style::SIZE}, Button::TOGGLE, logging, "Past Logs", Color::red};
_Task Logging::task{"Logging"};
bool Logging::active = true;

Logging tracking_log {"Tracking" , false};
Logging drive_log    {"Drive"    , false};
Logging flywheel_log {"Flywheel" , true , log_locations::sd_main};
Logging state_log    {"State"    , true , log_locations::sd_only};
Logging subsystem_log{"Subsystem", true , log_locations::sd_main};
Logging system_log   {"System"   , true , log_locations::sd_main, term_colours::ERROR};
Logging important_log{"Important", false, log_locations::sd_only};
Logging none_log     {"None"     , false, log_locations::none};
//!If more logs are created, the number of past logs will need to be reduced


Logging::Logging(std::string name, bool newline, log_locations location, term_colours print_colour):
ObjectTracker{"Logging", name}, queue{name}, newline{newline}, location{location}, print_colour{print_colour} {
  static int x = 130, y = 40;
  print_btn.construct({x, y, 100, 40, GUI::Style::SIZE}, Button::SINGLE, &logging, name, Color::dark_orange, Color::black);
  x = x != 360 ? x+115 : 15;
  if ((getID()+1) % 4 == 0) y += 50;

  print_btn.setFunc([this](){
    auto file = Interrupter<std::ifstream>(past_logs.isOn() ? pastFullName : fullName, std::ifstream::in);
    printf2("\n\n\n||||||||||||||||||||||||||||||||||||||||||||||\n");
    printf2(term_colours::GREEN, "Start %s Log Terminal Dump", getName());
    printf2("%s\n", getTermColour(term_colours::BLUE));

    if(file.stream.is_open()) std::cout << file.stream.rdbuf() << std::endl;
    else printf2("%s unopenable\n", past_logs.isOn() ? pastFullName : fullName);

    printf2(term_colours::RED, "\nEnd %s Log Terminal Dump", getName());
    printf2("\n||||||||||||||||||||||||||||||||||||||||||||||\n\n");
  });
}

void Logging::init(){
  system_log("%d: Initializing Logging", millis());
  if(!usd::is_installed()){ //Rerouting data to non-sd card
    alert::start("SD Logging Inactive");
    system_log(term_colours::ERROR, "No SD Card, deactivating Logging");
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
    int count, next_count, past_count;
    {
      std::ifstream log_count{"/usd/log_count.txt"};
      if (log_count.is_open()) log_count >> count;
      else count = 0;
      next_count = count >= 10 ? 0 : count + 1;
      past_count = count == 0 ? 10 : count - 1;
    }
    {
      system_log(true, "Log Version %d", count);
      std::ofstream log_count{"/usd/log_count.txt"};
      log_count << next_count; //For the next round of logs
    }

    for(Logging* log: getList()){
      if(log->location == log_locations::sd_main || log->location == log_locations::sd_only || log->location == log_locations::both){
        log->fullName = "/usd/Logging/" + std::to_string(count) + '_' + log->getName() + ".txt";
        log->pastFullName = "/usd/Logging/" + std::to_string(past_count) + '_' + log->getName() + ".txt";
        system_log("%d: Opening %s log file on SD", millis(), log->fullName);
        std::ofstream file_init{log->fullName, std::ofstream::trunc};
        file_init << "Start of " + log->getName() + " - " + std::to_string(count) + " log file\n\n";
      }
    }
  }

  for(Logging* log: getList()){
    if(log->location == log_locations::terminal || log->location == log_locations::none) log->print_btn.setActive(false);
  }

  task.start([](){ //Logging is good to go
    Timer timer{"Logging Queue"};
    while(true){
      if(timer.getTime() > max_time){
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
  if(!queue.empty() && (force || queue.size() > max_size)){
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

void Logging::pause(){
  system_log(term_colours::RED, "%d: Pausing Logging", millis());
  task.suspend();
  active = false;
}
void Logging::resume(){
  system_log(term_colours::GREEN, "%d: Resuming Logging", millis());
  task.resume();
  active = true;
}
// bool Logging::paused() {return task.getState() == TASK_STATE_SUSPENDED;}