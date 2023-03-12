#pragma once
// #include "logging.hpp"
#include "main.h"
#include "printing.hpp"
#include <cstddef>
#include <sstream>

template <typename Derived, std::size_t size = std::numeric_limits<std::size_t>::max()>
class ObjectTracker{
  private:
    ObjectTracker(ObjectTracker const &) = delete;
    ObjectTracker& operator=(ObjectTracker const &) = delete;

    std::size_t id;
    std::string short_name, name, full_name;

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

    ObjectTracker(std::string class_name, std::string name): name{name}{
      if(getList().size() < size){
        getListInternal().push_back(static_cast<Derived*>(this));
        id = getList().size();
        full_name = name + ' ' + class_name;

        std::stringstream ss{getName()};
        using iterator = std::istream_iterator<std::string>;
        for(auto it = iterator{ss}; it != iterator{}; it++) short_name += it->front();

        // device_log("Initialized %s, object %d of %s class.", name, id, class_name);
      }
      else throw std::length_error(sprintf2("Creating \"%s\" would exceed class' object limit of %d", full_name, size));
    }

  public:
    static Derived* getNth(std::size_t n) {return (n < getList().size()) ? getList()[n] : nullptr;}
    static std::vector<Derived*> const & getList() {return getListInternal();}

    std::size_t getID()        const {return id;}
    std::string getShortName() const {return short_name;}
    std::string getName()      const {return       name;}
    std::string getFullName()  const {return  full_name;}
};