#pragma once
#include "queue.hpp"

namespace alert{
  struct Params{
    term_colours colour;
    Color GUI_colour;
    std::uint32_t time;

    Params(term_colours colour = term_colours::ERROR, std::uint32_t time = 1000): colour{colour}, GUI_colour{getGUIColour(colour)}, time{time} {}
    Params(std::uint32_t time): Params{term_colours::ERROR, time} {}
  };

  void update();

  extern Queue<std::pair<Params, std::string>, 25> queue;

  void start   (Params params, std::string fmt, auto... args) {queue.         push({params, sprintf2(fmt, args...)});}
  void start   (std::string fmt, auto... args)                {queue.         push({Params{}, sprintf2(fmt, args...)});}
  void priority(Params params, std::string fmt, auto... args) {queue.priority_push({params, sprintf2(fmt, args...)});}
  void priority(std::string fmt, auto... args)                {queue.priority_push({Params{}, sprintf2(fmt, args...)});}
}