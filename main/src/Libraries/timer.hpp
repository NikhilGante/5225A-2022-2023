#pragma once
#include "main.h"
#include "logging.hpp"

enum class timing_units{
  millis,
  micros
};

class Timer{
  // 'time' is the time counted by the timer until the last call of pause()
  uint64_t last_reset_time, time, last_play_time;
  const char* name;
  bool paused;    // state of timer
  timing_units timing_unit;
  Logging log;
  uint64_t getTimeInTimingUnit(); // returns time in either millis micros

public:

  Timer(const char* name, Logging log = misc, bool play = true, timing_units timing_unit = timing_units::millis);
  void reset(bool play = true);
  uint64_t getTime();
  void play();
  void pause();
  void print(const char* str = ""); // prints to the terminal with custom message
  bool playing() const;
};
