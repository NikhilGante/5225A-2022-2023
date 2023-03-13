#include "logging.hpp"

void trackerInitMessage(std::string name, std::size_t id, std::string class_name){
  state_log("Initialized %s%sobject %d of %s class.", name, name != "" ? ", " : "", id, class_name);
}