#pragma once
#include "main.h"


class Timer2{
  // 'time' is the time counted by the timer until the last call of pause()
  int lReset = 0, time = 0, lPlay = 0, lPause = 0;
  std::string name;
  bool running;    // state of timer
  int getProgramTime(); // returns time in either millis micros

public:

  Timer2(std::string name, bool run = true);

  int getLastResetTime();
  int getLastPlayTime();
  int getLastPauseTime();

  void reset(bool run = true);
  int get_time();
  void play();
  void pause();
  //void print(const char* str = ""); // prints to the terminal with custom message
  bool curRunning() const;
};