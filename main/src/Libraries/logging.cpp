#include "logging.hpp"
#include "timer.hpp"
#include "task.hpp"
#include <fstream>

std::string Data::file_name{"/usd/data.txt"};
_Task_ Data::task{"logging"};
Queue<char, 20000> Data::queue{"Logging"};
std::vector<Data*> Data::obj_list;
Mutex Data::mutex;

Data task_log("tasks");
Data state_log("states", log_locations::sd);
Data controller_queue("controller", log_locations::t, term_colours::NONE, true);
Data tracking_data("tracking");
Data misc("misc", log_locations::both);
Data term("terminal", log_locations::t);
Data log_d("log", log_locations::sd);
Data error("error", log_locations::both, term_colours::ERROR);

Data::Data(std::string name, log_locations log_location, term_colours print_colour, bool newline):
name{name + ".txt"}, log_location{log_location}, newline{newline}, print_colour{print_colour} {obj_list.push_back(this);}

void Data::init(){
  using std::ofstream;
  {
    ofstream file{file_name, ofstream::trunc};
    if(!file.is_open()){
      printf2(term_colours::ERROR, "Log File not found");
      for(Data* obj: obj_list){
        log_locations& loc = obj->log_location;
        if(loc == log_locations::sd || loc == log_locations::both) loc = log_locations::t;
      }
    }
  } //To limit scope of ofstream file

  task.start([](){
    Timer timer{"logging_tmr"};

    while(true){
      if(!queue.empty() && (timer.get_time() > print_max_time || queue.size() > print_max_size)){
        mutex.take();
        ofstream file{file_name, ofstream::app};
        mutex.take();
        queue.output(file);
        mutex.give();

        timer.reset();
      }

      _Task_::delay(10);
    }
  });
}