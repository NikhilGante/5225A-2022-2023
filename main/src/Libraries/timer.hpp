#pragma once
#include "main.h"

class Logging;
extern Logging none_log;

enum class timing_units{
  millis,
  micros
};

class Timer{
  private:
    // 'time' is the time counted by the timer until the last call of pause()
    uint64_t last_reset_time, time, last_play_time;
    std::string name;
    bool paused;    // state of timer
    timing_units timing_unit;
    Logging* log;
    uint64_t getTimeInTimingUnit() const; // returns time in either millis micros

    Timer(Timer const &) = delete;
    Timer& operator=(Timer const &) = delete;

  public:
    Timer(std::string name, Logging& log = none_log, bool play = true, timing_units timing_unit = timing_units::millis);
    void reset(bool play = true);
    uint64_t getTime() const;
    void play();
    void pause();
    bool playing() const;
    std::string get_name() const;
    void print(std::string str = "", auto... args) const {
      (*log)("%s%s Timer's current time is: %lld | %s\n", str == "" ? "" : sprintf2(str, args...) + " | ", get_name(), getTime());
    }
};
