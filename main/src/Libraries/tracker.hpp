#pragma once
// #include "logging.hpp"
#include "main.h"
#include "printing.hpp"

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
        // device_log("Initialized object %d of an ObjectTracker subclass.", id);
      }
      else throw std::length_error("Too many objects are being created in an ObjectTracker subclass.");
    }

    ObjectTracker(std::string class_name, std::string name){
      if(getList().size() < size){
        getListInternal().push_back(static_cast<derived*>(this));
        id = getList().size();
        // device_log("Initialized %s, object %d of %s class.", name, id, class_name);
      }
      else throw std::length_error(sprintf2("Creating \"%s\" would exceed %s's object limit of %d", name, class_name, size));
    }

  public:
    static derived* getNth(std::size_t n) {return (n < getList().size()) ? getList()[n] : nullptr;}
    static std::vector<derived*> const & getList() {return getListInternal();}

    std::size_t getID() const {return id;}
};