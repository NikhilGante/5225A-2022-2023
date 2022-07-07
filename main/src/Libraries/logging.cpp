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
  printf("\n");
  if(logfile == NULL) return;
  logfile = fopen("/usd/log.txt","a");
  vfprintf(logfile, format, arguments);
  fclose(logfile);
  va_end(arguments);
}