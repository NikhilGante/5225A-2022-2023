#include "logging.hpp"
#include "timer.hpp"
std::string Data::file_name{"/usd/data.txt"};
Queue<char, 1024> Data::queue{};
vector<Data*> Data::obj_list;
_Task_ Data::log_t("logging");

Data task_log("tasks.txt");
// Data controller_queue("controller.txt","$02", general,log_locations::none);
Data tracking_data("tracking.txt");
// Data tracking_imp("tracking.txt","$03",general,log_locations::both);
// Data misc("misc.txt", "$04",general,log_locations::both);
// Data drivers_data("driver.txt", "$05", general,log_locations::none);
// Data motion_i("motion.txt","$06",general,log_locations::both);
// Data motion_d("motion.txt", "$06", general,log_locations::t);
// Data term("terminal.txt","$07",general,log_locations::t);
Data log_d("log.txt", log_locations::sd, false);
// Data graph("graph.txt","$09",general,log_locations::sd, term_colours::NONE, -1);
// Data state_log_d.print("state.txt", "$11", general,log_locations::both);
// Data ERROR("error.txt", "$12", error, log_locations::both, term_colours::ERROR);
// Data skills_d("skills.txt", "$13", general, log_locations::both);
// Data safety("safety.txt", "$14", general, log_locations::both);

Data::Data(std::string name, log_locations log_location, bool newline, term_colours print_colour):
name{name}, log_location{log_location}, newline{newline}, print_colour{print_colour} {obj_list.push_back(this);}

void Data::init(){
  ofstream file{file_name, ofstream::trunc};

  if(!file.is_open()){
    printf2(term_colours::ERROR, "Log File not found");
    for(Data* obj: Data::obj_list){
      if(obj->log_location == log_locations::sd || obj->log_location == log_locations::both) obj->log_location = log_locations::t;
    }
  }

  Data::log_t.start(queue_handle);
}

void Data::queue_handle(){
  Timer timer{"logging_tmr"};

  while(true){
    if(!queue.empty() && (queue.size() > print_point || timer.get_time() > print_max_time)){
      ofstream file{file_name, ofstream::app};
      queue.output(file);
      timer.reset();
    }
    _Task_::delay(10);
  }
}