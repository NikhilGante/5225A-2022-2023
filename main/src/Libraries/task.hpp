#pragma once
#include "../util.hpp"
#include "logging.hpp"

class TaskEndException: public std::exception{
  public: const char* what();
};

enum class notify_types_2{
  none, // no notification received
  interrupt,  // kills the task safely
  suspend,  // suspends the task
};

class _Task{
  private:
    task_t task_handle = 0; // handle to the pros task
    std::string name;

  public:
    _Task(std::string name = "");
    template <std::invocable F>
    void start(F&& function, void* parameters = nullptr, uint8_t prio = TASK_PRIORITY_DEFAULT, uint16_t stack_depth = TASK_STACK_DEPTH_DEFAULT){
      system_log(term_colours::NOTIF, "Starting %s task", name);
      kill(); // kills task if it's alive
      task_handle = Task::create(std::forward<F>(function), prio, stack_depth, name.c_str());
      system_log(term_colours::GREEN, "Started %s task", name);
    }
    bool isAlive() const; // returns if the task is currently on the scheduler
    void killUnsafe(); // removes task from scheduler
    void kill();  // sends a notification to kill the task
    void suspend();  // sends a notification to suspend the task
    void resume();  // resume the task
    task_state_e_t getState() const;
    
    static void delay(uint32_t delay_time = 10);  // special delay that handles notifications
};
