#include "timer2.hpp"

Timer2::Timer2(std::string name, bool run): 
name(name)
{
  reset(run);
}
int getProgramTime(){
  return millis();
}
int Timer2::getLastResetTime(){
  return lReset;
}
int Timer2::getLastPlayTime(){
  return lPlay;
}
int Timer2::getLastPauseTime(){
  return lPause;
}

void Timer2::reset(bool run){
  lReset = getProgramTime();
  time = 0;
  running = run;
}


int Timer2::get_time(){
  if (running){
    return time + getProgramTime()-lPlay;
  } else return time;
}


void Timer2::play(){
  running = true;
  lPlay = getProgramTime();

}
void Timer2::pause(){
  running = false;
  if (running){
    time += getProgramTime()-std::max(lReset, lPlay);
    lPause = getProgramTime();
  } else{
    printf("%s time is already paused", name);
  }
  
  
}
bool Timer2::curRunning() const{
  return running;
}


// reset = 0 // 0ms

// pause = 200// 200ms time = 200
// // This 100ms is not part of the time -----------------
// play = 300 //300ms

// pause // 450ms time = 350
// play // 500ms
// get_time // 600ms = time(350) + ProgramTime(600)-LastPlayTime



