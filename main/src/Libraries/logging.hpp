#pragma once
#include "main.h"
#include <fstream>
#include <stdarg.h>

extern FILE* logfile;

void log(const char * format, ...);
void log_init();