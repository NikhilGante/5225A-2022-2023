#pragma once
#include "main.h"
#include "okapi/api/util/mathUtil.hpp"
#include "pros/colors.hpp"
#include <type_traits>

struct Position;
struct Vector;

constexpr int n_printf_max = 50;

//feel free to suggest a shorter name for term_colours
enum class term_colours{
  NONE = 0,
  BLACK,
  RED,
  GREEN,
  YELLOW,
  BLUE,
  MAGENTA,
  CYAN,
  WHITE,
  ERROR,
  WARNING,
  NOTIF,
};

std::string getTermColour(term_colours colour);
Color getGUIColour(term_colours);

void newline(int count = 1);

//Convert Args
  /*General case*/ std::string convert_all_args(std::string const & fmt, auto const & arg) requires (std::is_trivial_v<decltype(arg)>);
  /*For arithmetic types*/ std::string convert_all_args(std::string const & fmt, auto arg) requires (std::is_arithmetic_v<decltype(arg)>);
  /*Enums*/ std::string convert_all_args(std::string const & fmt, auto arg) requires (std::is_enum_v<decltype(arg)>);
  /*Ranges*/ std::string convert_all_args(std::string const & fmt, const auto& arg) requires requires{arg.begin();};
  /*Arrays (C)*/ template <typename _Tp, std::size_t _Nm> std::string convert_all_args(std::string const & fmt, const _Tp (&arg) [_Nm]);
  /*Pointers */ std::string convert_all_args(std::string const & fmt, auto* arg);
  /*Strings*/ std::string convert_all_args(std::string const & fmt, std::string const & arg);
  /*Positions*/ std::string convert_all_args(std::string const & fmt, const Position& arg);
  /*Vectors (Geometry)*/ std::string convert_all_args(std::string const & fmt, const Vector& arg);

// Printing

  std::string const & sprintf2(std::string const & fmt); //Template Recursion Base case
  std::string sprintf2(std::string fmt, const auto& arg, const auto&... args);
  std::string sprintf2_colour(term_colours colour, std::string fmt, auto... args);
  void printf2(term_colours colour, std::string fmt, auto... args);
  void printf2(std::string fmt, auto... args);

//Convert Args Definitions
  std::string convert_all_args(std::string const & fmt, auto const & arg) requires (std::is_trivial_v<decltype(arg)>){ //General
    char buffer[n_printf_max];
    snprintf(buffer, n_printf_max, fmt.c_str(), arg);
    return buffer;
  }
  std::string convert_all_args(std::string const & fmt, auto arg) requires (std::is_arithmetic_v<decltype(arg)>){ //Arithmetic
    const char* format = fmt.c_str();
    std::string fmt_safe = "   " + fmt;
    char buffer[n_printf_max];
    char end[3] = {static_cast<char>(tolower(*(fmt.end() - 1))), static_cast<char>(*(fmt.end() - 2)), static_cast<char>(*(fmt.end() - 3))};

    if(end[1] == 'L') snprintf(buffer, n_printf_max, format, static_cast<long double>(arg));
    else if(end[0] == 'f' || end[0] == 'e' || end[0] == 'g' || end[0] == 'a') snprintf(buffer, n_printf_max, format, static_cast<double>(arg));
    else if(end[0] == 'd' || end[0] == 'i'){
      if(end[2] == 'h') snprintf(buffer, n_printf_max, format, static_cast<signed char>(arg));
      else if(end[2] == 'l') snprintf(buffer, n_printf_max, format, static_cast<long long int>(arg));
      else if(end[1] == 'h') snprintf(buffer, n_printf_max, format, static_cast<short int>(arg));
      else if(end[1] == 'l') snprintf(buffer, n_printf_max, format, static_cast<long int>(arg));
      else if(end[1] == 'j') snprintf(buffer, n_printf_max, format, static_cast<intmax_t>(arg));
      else snprintf(buffer, n_printf_max, format, static_cast<int>(arg));
    }
    else if(end[0] == 'u' || end[0] == 'o' || end[0] == 'x'){
      if(end[2] == 'h') snprintf(buffer, n_printf_max, format, static_cast<unsigned char>(arg));
      else if(end[2] == 'l') snprintf(buffer, n_printf_max, format, static_cast<unsigned long long int>(arg));
      else if(end[1] == 'h') snprintf(buffer, n_printf_max, format, static_cast<unsigned short int>(arg));
      else if(end[1] == 'l') snprintf(buffer, n_printf_max, format, static_cast<unsigned long int>(arg));
      else if(end[1] == 'j') snprintf(buffer, n_printf_max, format, static_cast<uintmax_t>(arg));
      else snprintf(buffer, n_printf_max, format, static_cast<unsigned int>(arg));
    }
    else if(end[0] == 'c'){
      if(end[1] == 'l') snprintf(buffer, n_printf_max, format, static_cast<wint_t>(arg));
      else snprintf(buffer, n_printf_max, format, static_cast<int>(arg));
    }
    else if(end[1] == 'z') snprintf(buffer, n_printf_max, format, static_cast<std::size_t>(arg));
    else if(end[1] == 't') snprintf(buffer, n_printf_max, format, static_cast<ptrdiff_t>(arg));

    else snprintf(buffer, n_printf_max, format, arg);
    return buffer;
  }
  std::string convert_all_args(std::string const & fmt, auto arg) requires (std::is_enum_v<decltype(arg)>){ //General
    return convert_all_args(fmt, okapi::toUnderlyingType(arg));
  }

  std::string convert_all_args(std::string const & fmt, auto* arg){
    char buffer[n_printf_max];
    snprintf(buffer, n_printf_max, fmt.c_str(), arg);
    return buffer;
  }

  std::string convert_all_args(std::string const & fmt, const auto& arg) requires requires{arg.begin();} { //Ranges
    if(fmt.back() == 'p' || (std::same_as<typename decltype(arg)::value_type, char> && fmt.back() == 's')) return convert_all_args(fmt, arg.data());
    std::string str = "{";
    for (auto it = arg.cbegin(); it != arg.cend(); it++){
      str += convert_all_args(fmt, *it);
      if (it+1 != arg.end()) str += ", ";
    }
    str += '}';
    return str;
  }

  template <typename _Tp, std::size_t _Nm>
  std::string convert_all_args(std::string const & fmt, const _Tp (&arg) [_Nm]){
    if(fmt.back() == 'p') return convert_all_args(fmt, const_cast<_Tp*>(arg));
    else if(std::is_same<_Tp, char>::value && fmt.back() == 's') return convert_all_args(fmt, const_cast<_Tp*>(arg));
    std::string str;
    str += '{';
    for (int i = 0; i < _Nm; i++){
      str += convert_all_args(fmt, arg[i]);
      if (i+1 != _Nm) str += ", ";
    }
    str += '}';
    return str;
  }

//Print Definitions
  std::string sprintf2(std::string fmt, const auto& arg, const auto&... args){
    std::string::const_iterator first = fmt.begin(), second; //initialized just to be overwritten

    first = std::find(first, fmt.cend(), '%');
    second = fmt.begin() + fmt.find_first_of("diuoxXfFeEgGaAcspn%*", first-fmt.begin()+1) + 1;
    
    if(second == fmt.begin()) return fmt; //No valid format specifier found

    std::string start = std::string(fmt.cbegin(), first);
    std::string format_specifier = std::string(first, second);
    std::string converted;
    std::string rest = std::string(second, fmt.cend());

    //convert to switch
    if(format_specifier.back() == '%'){
      format_specifier.pop_back();
      converted = "%";
      rest = sprintf2(rest, arg, args...);
    }
    else if(format_specifier.back() == '*'){
      format_specifier.pop_back();
      format_specifier += convert_all_args("%d", arg);
      rest = sprintf2(format_specifier + rest, args...);
    }
    else if(format_specifier.back() == 'n'){
      converted = "";
      rest = sprintf2(rest, args...);
    }
    else{
      converted = convert_all_args(format_specifier, arg);
      rest = sprintf2(rest, args...);
    }
    
    return start + converted + rest;
  }
  std::string sprintf2_colour(term_colours colour, std::string fmt, auto... args){
    std::string str = sprintf2(fmt, args...);

    int white_front_count = str.find_first_not_of(" \n");
    std::string white_front_space = std::string(str.begin(), str.begin() + white_front_count);
    str.erase(str.begin(), str.begin() + white_front_count);

    int white_end_count = str.find_last_not_of(" \n");
    std::string white_end_space = std::string(str.begin() + white_end_count + 1, str.end());
    str.erase(str.begin() + white_end_count + 1, str.end());

    return white_front_space + getTermColour(colour) + str + getTermColour(term_colours::NONE) + white_end_space;
  }
  void printf2(term_colours colour, std::string fmt, auto... args){
    printf("%s", sprintf2_colour(colour, fmt, args...).c_str());
  }
  void printf2(std::string fmt, auto... args){
    printf("%s", sprintf2(fmt, args...).c_str());
  }