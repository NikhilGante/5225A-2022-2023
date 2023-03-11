#pragma once
// #include "logging.hpp"
#include "main.h"
#include "printing.hpp"

template <typename Derived, std::size_t size = std::numeric_limits<std::size_t>::max()>
class ObjectTracker{
  private:
    ObjectTracker(ObjectTracker const &) = delete;
    ObjectTracker& operator=(ObjectTracker const &) = delete;

    std::size_t id;
    static std::vector<Derived*> & getListInternal() {static std::vector<Derived*> objects; return objects;}

  protected:
    ObjectTracker(){
      if(getList().size() < size){
        getListInternal().push_back(static_cast<Derived*>(this));
        id = getList().size();
        // device_log("Initialized object %d of an ObjectTracker subclass.", id);
      }
      else throw std::length_error("Too many objects are being created in an ObjectTracker subclass.");
    }

    ObjectTracker(std::string label){
      if(getList().size() < size){
        getListInternal().push_back(static_cast<Derived*>(this));
        id = getList().size();
        // device_log("Initialized %s, object %d of %s class.", name, id, class_name);
      }
      else throw std::length_error(sprintf2("Creating \"%s\" would exceed class' object limit of %d", label, size));
    }

  public:
    static Derived* getNth(std::size_t n) {return (n < getList().size()) ? getList()[n] : nullptr;}
    static std::vector<Derived*> const & getList() {return getListInternal();}

    std::size_t getID() const {return id;}
};