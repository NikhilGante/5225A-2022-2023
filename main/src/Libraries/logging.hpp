#pragma once
#include "main.h"
#include <fstream>
#include <stdarg.h>

#include "pros/rtos.hpp"
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
  // static Queue<char, QUEUE_SIZE> queue;
  static char queue[QUEUE_SIZE];
  static _Task task;
  static void logHandle(); // runs in task to flush out contents of queue to file
  static void print_queue(const char queue[QUEUE_SIZE],  ofstream& log_file, std::string file_name);
  static size_t queue_size;
  static Timer log_timer;
  static pros::Mutex log_mutex;
public:
  static ofstream log_file;
  static void init(); // starts log task

  static E_Log_Levels g_log_level;

  E_Log_Locations log_location;
  E_Log_Levels log_level;

  void print(std::string str);

};

