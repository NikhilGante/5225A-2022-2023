#include "logging.hpp"
#include "timer.hpp"
#include "task.hpp"
#include <fstream>


std::string Data::file_name{"/usd/data.txt"};
Queue<char, 20000> Data::queue{"Logging"};
vector<Data*> Data::obj_list;
_Task_ Data::log_t("logging");

Data task_log("tasks");
Data controller_queue("controller", log_locations::t, term_colours::NONE, true);
Data tracking_data("tracking");
Data misc("misc", log_locations::both);
Data term("terminal", log_locations::t);
Data log_d("log", log_locations::sd);
Data error("error", log_locations::both, term_colours::ERROR);

Data::Data(std::string name, log_locations log_location, term_colours print_colour, bool newline):
name{name + ".txt"}, log_location{log_location}, newline{newline}, print_colour{print_colour} {obj_list.push_back(this);}

void Data::init(){
  ofstream file{file_name, ofstream::trunc};

  if(!file.is_open()){
    printf2(term_colours::ERROR, "Log File not found");
    for(Data* obj: obj_list){
      if(obj->log_location == log_locations::sd || obj->log_location == log_locations::both) obj->log_location = log_locations::t;
    }
  }

  log_t.start([](){
    Timer timer{"logging_tmr"};

    while(true){
      if(!queue.empty() && (timer.get_time() > print_max_time || queue.size() > print_max_size)){
        ofstream file{file_name, ofstream::app};
        queue.output(file);
        timer.reset();
      }

      _Task_::delay(10);
    }
  });
}