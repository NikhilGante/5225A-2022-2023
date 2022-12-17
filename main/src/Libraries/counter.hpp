#pragma once
#include "main.h"
#include "gui.hpp"

template <typename derived, std::size_t size = 0>
class Counter{
  private:
    Counter(Counter const &) = delete;
    Counter& operator=(Counter const &) = delete;

    static std::size_t total;
    static std::vector<derived*> objects;

    std::size_t count;

  protected:
    Counter(std::string name = ""){
      if(size == 0 || (size && total < size)){
        objects.push_back(static_cast<derived*>(this));
        count = total++;
      }
      else if (name != "") alert::start("Too many objects of type are being created. Failed upon creation of \'%s\'", name);
      else alert::start("Too many objects of type are being created.");
    }

  public:
    static derived* getNth(std::size_t n) {return (n < getList().size()) ? getList()[n] : nullptr;}
    static std::size_t getCount() {return total;}
    static std::vector<derived*> const & getList() {return objects;}

    std::size_t getID() {return count;}
};

template <typename derived, std::size_t size> std::size_t Counter<derived, size>::total;
template <typename derived, std::size_t size> std::vector<derived*> Counter<derived, size>::objects;