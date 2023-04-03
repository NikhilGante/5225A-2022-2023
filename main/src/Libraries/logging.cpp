#include "logging.hpp"
#include "../config.hpp"
#include <cstring>
using namespace std;
using namespace pros;
FILE* logfile = NULL;

Data LOG;
pros::Mutex log_mutex;

void log_init() {
  logfile = fopen("/usd/log.txt","w");
  if(logfile==NULL) {printf("could not open logfile\n"); return;}
  else printf("logfile found\n");
  if(logfile != NULL)fprintf(logfile, "\n>>>>>START LOGGING FOR PROGRAM\n");
  fclose(logfile);

  Data::init();
}

void log(const char * format, ...){
  const int buffer_size = 256;  // max amount of chars allowed to be printed
  char buffer[buffer_size];
  va_list args;
  va_start (args, format);
  int chars_printed = vsnprintf(buffer, buffer_size, format, args);  // prints formatted string to buffer
  if(chars_printed > buffer_size) printf("Only %d chars printed\n", chars_printed);
  va_end (args);


  LOG.print(buffer);
}


// ACTUAL LOGGING START

// static data members
E_Log_Levels Data::g_log_level = E_Log_Levels::debug;
// Queue<char, QUEUE_SIZE> Data::queue("log queue");
char Data::queue[QUEUE_SIZE];
pros::Mutex Data::log_mutex;
size_t Data::queue_size = 0;
_Task Data::task("log_task");
Timer Data::log_timer("log_timer");

ofstream Data::log_file;

void Data::init(){
  task.start(logHandle);
}

void Data::logHandle(){ // runs in task to flush out contents of queue to file
  log_timer.reset();
  try{
    while(true){
      if(queue_size > 2000 || log_timer.getTime() > 500 && queue_size != 0){
        
        log_mutex.take();
        if (pros::usd::is_installed()) print_queue(queue, log_file, "/usd/log.txt");
        else master.rumble("-");
        log_mutex.give();
        
        log_timer.reset();
      }
      _Task::delay(10);
    }
  }
  catch(const TaskEndException& exception){
    print_queue(queue, log_file, "/usd/log.txt");  // empty the queue when the task gets killed
  }
}

void Data::print_queue(const char queue[QUEUE_SIZE],  ofstream& log_file, std::string file_name){
  long long int t = micros();
  log_file.open(file_name, ios::app);
  log_file.write(queue, min<int32_t>(queue_size, QUEUE_SIZE));  // prints from front to rear of queue
  log_file.close();
  long long int t1 = micros();
  printf("FLUSH, Took: %lld at: %d, chars printed: %d\n", t1-t, millis(), queue_size);
  queue_size = 0;
}

void Data::print(std::string str){
  int chars_printed = str.length();
  

  log_mutex.take();
  printf("%s", str.c_str());
  memcpy(queue+queue_size, str.c_str(), chars_printed);
  queue_size += chars_printed;
  log_mutex.give();
}


