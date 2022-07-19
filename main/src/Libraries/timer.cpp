#include "timer.hpp"

Timer::Timer(const char* name, bool play, timing_units timing_unit):
name(name), timing_unit(timing_unit)
{
  printf("%s's initialize time is: %lld\n", name, getTimeInTimingUnit());
  reset(play);
}

uint64_t Timer::getLastResetTime(){
    return last_reset_time;
}

void Timer::reset(bool play){
  time = 0;
  if(play){
    paused = true;
    this->play();
  }
  else  paused = true;
}

uint64_t Timer::get_time(){
  if (paused) return time;
  else  return getTimeInTimingUnit() - last_play_time + time;
}

void Timer::play(){
  if (paused){
    last_play_time = getTimeInTimingUnit();
    paused = false;
  }
  else  printf("Timer \"%s\" is already playing.\n", name);
}

void Timer::pause(){
  if (!paused){
    time += getTimeInTimingUnit() - last_play_time;
    paused = true;
  }
  else  printf("Timer \"%s\" is already paused.\n", name);
}

void Timer::print(const char* str){
  printf("%s's current time is: %lld | %s\n", name, get_time(), str);
}

uint64_t Timer::getTimeInTimingUnit(){ // returns time in either millis or micros
  return pros::micros() * (timing_unit == timing_units::micros ? 1 : 0.001);
}
