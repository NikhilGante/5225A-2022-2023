#include "timer.hpp"
#include "logging.hpp"

Timer::Timer(std::string name, Logging& log, bool play, timing_units timing_unit):
name{name}, timing_unit{timing_unit}, log{&log} {
  log(get_name() + "\'s initialize time is: %lld\n", getTimeInTimingUnit());
  reset(play);
}

void Timer::reset(bool play){
  time = 0;
  paused = true;
  (*log)("Resetting " + get_name() + "\n");
  if(play) this->play();
}

void Timer::play(){
  if (paused){
    last_play_time = getTimeInTimingUnit();
    paused = false;
  }
  else (*log)(get_name() + " is already playing.\n");
}

void Timer::pause(){
  if (!paused){
    time += getTimeInTimingUnit() - last_play_time;
    paused = true;
  }
  else (*log)(get_name() + " is already paused.\n");
}

std::string Timer::get_name() const {return name + " Timer";}
bool Timer::playing() const {return !paused;}
uint64_t Timer::getTime() const {return paused ? time : getTimeInTimingUnit() - last_play_time + time;}
uint64_t Timer::getTimeInTimingUnit() const {return micros() * (timing_unit == timing_units::micros ? 1 : 0.001);} //Returns time in either millis or micros
