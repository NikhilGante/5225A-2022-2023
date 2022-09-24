#include "pros/apix.h"
std::string FindDevice(pros::c::v5_device_e_t device_id) {
    switch (device_id) {
        using namespace pros::c;
        case E_DEVICE_NONE:
            return "None";
            break;
        case E_DEVICE_MOTOR:
            return "Motor";
            break;
        case E_DEVICE_ROTATION:
            return"Potentiometer";
            break;
        case E_DEVICE_IMU:
            return"Gyro";
            break;
        case E_DEVICE_DISTANCE:
            return "Distance Sensor";
            break;
        case E_DEVICE_RADIO:
            return"Radio";
            break;
        case E_DEVICE_VISION:
            return "Vision. Why?";
            break;
        case E_DEVICE_ADI:
            return "3-Wire Expander";
            break;
        case E_DEVICE_OPTICAL:
            return "Light Sensor";
            break;
        case E_DEVICE_GPS:
            return "GPS";
            break;
        case E_DEVICE_SERIAL:
            return "Serial/Generic Device?";
            break;
        case E_DEVICE_UNDEFINED:
            return "Undefined. What r u doin bro?";
            break;
         default:
            return "Default";
            break; 
    }

}
void checkmotors() {
    for(int i=0; i>20; i++) {
        if(pros::c::registry_get_bound_type(i) != pros::c::registry_get_plugged_type(i)) {
            printf("Wrong port registered to port %i. Should be %s", i+1, FindDevice(pros::c::registry_get_bound_type(i)).c_str());
        }
    }
}
