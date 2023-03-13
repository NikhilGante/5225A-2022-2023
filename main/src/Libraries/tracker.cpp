#include "logging.hpp"

void trackerInitMessage(std::string name, std::size_t id, std::string class_name){
  device_log("Initialized %s, object %d of %s class.", name, id, class_name);
}