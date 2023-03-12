#pragma once
#include "queue.hpp"

namespace alert{
  void update();

  extern Queue<std::tuple<Color, term_colours, std::uint32_t, std::string>, 25> queue;

  void start   (                                         std::string fmt, auto... args) {queue.         push({getGUIColour(term_colours::ERROR), term_colours::ERROR, 1000, sprintf2(fmt, args...)});} //Defaults colour and time
  void start   (                     std::uint32_t time, std::string fmt, auto... args) {queue.         push({getGUIColour(term_colours::ERROR), term_colours::ERROR, time, sprintf2(fmt, args...)});} //Defaults colour
  void start   (term_colours colour,                     std::string fmt, auto... args) {queue.         push({getGUIColour(colour)             , colour             , 1000, sprintf2(fmt, args...)});} //Defaults time
  void start   (term_colours colour, std::uint32_t time, std::string fmt, auto... args) {queue.         push({getGUIColour(colour)             , colour             , time, sprintf2(fmt, args...)});} //Doesn't default

  void priority(                                         std::string fmt, auto... args) {queue.priority_push({getGUIColour(term_colours::ERROR), term_colours::ERROR, 1000, sprintf2(fmt, args...)});} //Defaults colour and time
  void priority(                     std::uint32_t time, std::string fmt, auto... args) {queue.priority_push({getGUIColour(term_colours::ERROR), term_colours::ERROR, time, sprintf2(fmt, args...)});} //Defaults colour
  void priority(term_colours colour,                     std::string fmt, auto... args) {queue.priority_push({getGUIColour(colour)             , colour             , 1000, sprintf2(fmt, args...)});} //Defaults time
  void priority(term_colours colour, std::uint32_t time, std::string fmt, auto... args) {queue.priority_push({getGUIColour(colour)             , colour             , time, sprintf2(fmt, args...)});} //Doesn't default
}