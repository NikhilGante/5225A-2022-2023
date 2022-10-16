#include "logging.hpp"
using namespace std;
using namespace pros;
FILE* logfile = NULL;


void log_init() {
  logfile = fopen("/usd/log.txt","w");
  if(logfile==NULL) {printf("could not open logfile\n"); return;}
  else printf("logfile found\n");
  if(logfile != NULL)fprintf(logfile, "\n>>>>>START LOGGING FOR PROGRAM\n");
  fclose(logfile);
}

void log(const char * format, ...){
  // mutex.take(50);
  va_list arguments;
  va_start(arguments,format);
  vprintf(format,arguments);
  // printf("\n");
  if(logfile == NULL) return;
  logfile = fopen("/usd/log.txt","a");
  vfprintf(logfile, format, arguments);
  fclose(logfile);
  va_end(arguments);
}

// ACTUAL LOGGING START

// static data members
E_Log_Levels Data::g_log_level = E_Log_Levels::debug;
Queue<char, QUEUE_SIZE> Data::queue("log queue");
_Task_ Data::task("log_task");
Timer Data::log_timer("log_timer");

ofstream Data::log_file;

void Data::init(){
  log_file.open("/usd/log.txt");
  log_file.close();
  // task.start(logHandle);
}

void Data::logHandle(){ // runs in task to flush out contents of queue to file
  log_timer.reset();
  try{
    while(true){
      if(queue.getDataSize() > 500 || log_timer.get_time() > 500){
        queuePrintFile(queue, log_file, "/usd/log.txt");
        log_timer.reset();
      }
      _Task_::delay(10);
    }
  }
  catch(const TaskEndException& exception){
    queuePrintFile(queue, log_file, "/usd/log.txt");  // empty the queue when the task gets killed
  }
}


