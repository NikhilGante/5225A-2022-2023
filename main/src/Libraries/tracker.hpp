#pragma once
#include "main.h"
#include "gui.hpp"

template <typename derived, std::size_t size = std::numeric_limits<std::size_t>::max()>
class ObjectTracker{
  private:
    ObjectTracker(ObjectTracker const &) = delete;
    ObjectTracker& operator=(ObjectTracker const &) = delete;

    std::size_t id;
    static std::vector<derived*> & getListInternal() {static std::vector<derived*> objects; return objects;}

  protected:
    ObjectTracker(){
      if(getList().size() < size){
        getListInternal().push_back(static_cast<derived*>(this));
        id = getList().size();
      }
      else throw std::length_error("Too many objects are being created in ObjectTracker subclass.");
    }

  public:
    static derived* getNth(std::size_t n) {return (n < getList().size()) ? getList()[n] : nullptr;}
    static std::vector<derived*> const & getList() {return getListInternal();}

    int getID() const {return id;}
};