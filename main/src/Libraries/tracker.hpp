#pragma once
#include "main.h"
#include "gui.hpp"

//! Figure out why vector goes to 0 with logging. Replace total with vector.size()
template <typename derived, std::size_t size = std::numeric_limits<std::size_t>::max()>
class ObjectTracker{
  private:
    ObjectTracker(ObjectTracker const &) = delete;
    ObjectTracker& operator=(ObjectTracker const &) = delete;

    static std::size_t total;
    static std::vector<derived*> objects;

    std::size_t count;

  protected:
    ObjectTracker(){
      if(total < size){
        objects.push_back(static_cast<derived*>(this));
        count = total++;
      }
      else throw std::length_error("Too many objects are being created in ObjectTracker subclass.");
    }

  public:
    static derived* getNth(std::size_t n) {return (n < getList().size()) ? getList()[n] : nullptr;}
    static std::size_t getCount() {return total;}
    static std::vector<derived*> const & getList() {return objects;}

    std::size_t getID() const {return count;}
};

template <typename derived, std::size_t size> std::size_t ObjectTracker<derived, size>::total;
template <typename derived, std::size_t size> std::vector<derived*> ObjectTracker<derived, size>::objects;