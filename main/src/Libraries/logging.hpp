#pragma once
#include "main.h"
#include <fstream>
#include <stdarg.h>

#include "queue.hpp"
#include "task.hpp"
#include "timer.hpp"

extern FILE* logfile;

extern pros::Mutex log_mutex;

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

enum class E_Log_Locations
{
  terminal,
  sd,
  both,
  none
};


class Data{
  static _Task task;
  static void logHandle(); // runs in task to flush out contents of queue to file
  static Timer log_timer;
public:
  static Queue<char, QUEUE_SIZE> queue;
  static ofstream log_file;
  static void init(); // starts log task

  static E_Log_Levels g_log_level;

  E_Log_Locations log_location;
  E_Log_Levels log_level;

  void print(const char* format, ...);

};

