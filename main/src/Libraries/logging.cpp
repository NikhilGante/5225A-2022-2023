#include "logging.hpp"
using namespace std;
using namespace pros;
FILE* logfile = NULL;

pros::Mutex log_mutex;

void log_init() {
  logfile = fopen("/usd/log.txt","w");
  if(logfile==NULL) {printf("could not open logfile\n"); return;}
  else printf("logfile found\n");
  if(logfile != NULL)fprintf(logfile, "\n>>>>>START LOGGING FOR PROGRAM\n");
  fclose(logfile);
}

void log(const char * format, ...){
  va_list arguments;
  va_start(arguments,format);
  vprintf(format,arguments);
  // printf("\n");
  if(logfile == NULL) return;
  log_mutex.take();
  logfile = fopen("/usd/log.txt","a");
  vfprintf(logfile, format, arguments);
  fclose(logfile);
  va_end(arguments);
  log_mutex.give();
}

// ACTUAL LOGGING START

// static data members
E_Log_Levels Data::g_log_level = E_Log_Levels::debug;
Queue<char, QUEUE_SIZE> Data::queue("log queue");
_Task Data::task("log_task");
Timer Data::log_timer("log_timer");

ofstream Data::log_file;

void Data::init(){
  task.start(logHandle);
  log_init();
}

void Data::logHandle(){ // runs in task to flush out contents of queue to file
  log_timer.reset();
  try{
    while(true){
      if(queue.getDataSize() > 2000 || (log_timer.getTime() > 500 && !queue.isEmpty())){
        queuePrintFile(queue, log_file, "/usd/log.txt");
        log_timer.reset();
      }
      _Task::delay(10);
    }
  }
  catch(const TaskEndException& exception){
    queuePrintFile(queue, log_file, "/usd/log.txt");  // empty the queue when the task gets killed
  }
}

void Data::print(const char* format, ...){
  const int buffer_size = 256;  // max amount of chars allowed to be printed
  char buffer[buffer_size];
  va_list args;
  va_start (args, format);
  int chars_printed = vsnprintf(buffer, buffer_size, format, args);  // prints formatted string to buffer
  if(chars_printed > buffer_size) printf("Only %d chars printed\n", chars_printed);
  va_end (args);

  queue.push(buffer, strlen(buffer)); // pushes buffer to queue
}


