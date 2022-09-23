#pragma once
#include "main.h"
#include <fstream>
#include <cstdarg>

#include "queue.hpp"
#include "task.hpp"
#include "timer.hpp"

extern FILE* logfile;

void log(const char * format, ...);
void log_init();

// ACTUAL LOGGING START

#define QUEUE_SIZE 10000

enum E_Log_Levels{
  error = 1,
  warning =1,
  general =1,
  debug = 2,
  off = 0,
};

enum class E_Log_Locations{
  terminal,
  sd,
  both,
  none
};


class Data{
  static Queue<char, QUEUE_SIZE> queue;
  static _Task_ task;
  static void logHandle(); // runs in task to flush out contents of queue to file
  static Timer log_timer;
public:
  static ofstream log_file;
  static void init(); // starts log task

  static E_Log_Levels g_log_level;

  E_Log_Locations log_location;
  E_Log_Levels log_level;

  //here just in case i want to make it a template
  inline void print(const char* format, ...){
    constexpr int buffer_size = 256;  // max amount of chars allowed to be printed
    char buffer[buffer_size];
    va_list args;
    va_start (args, format);
    int chars_printed = vsnprintf(buffer, buffer_size, format, args);  // prints formatted string to buffer
    if(chars_printed > buffer_size) printf("Only %d chars printed\n", chars_printed);
    va_end (args);

    printf("%s", buffer);
    queue.push(buffer, strlen(buffer)); // pushes buffer to queue
  }

};

