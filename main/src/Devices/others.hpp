#pragma once
#include "main.h"
#include "../Libraries/tracker.hpp"
#include "../config.hpp"
#include "pros/apix.h"

//These classes don't do much except wrap around exisiting sensor classes for iteration and port checking.

class Ultrasonic: private ADIUltrasonic, public ObjectTracker<Ultrasonic>{
  private:
    std::string name;
    Port port1, port2;

  public:
    Ultrasonic(Port port_ping, Port port_echo, std::string name);
    Ultrasonic(ext_adi_port_tuple_t port_tuple, std::string name);
    inline static const std::string class_name = "Ultrasonic";


    std::string getName() const;
    ext_adi_port_pair_t getPorts() const;
    int getVal() const;
};

class Light: private ADIAnalogIn, public ObjectTracker<Light>{
  private:
    std::string name;
    Port port;

  public:
    Light(Port port, std::string name);
    Light(ext_adi_port_pair_t port_pair, std::string name);
    inline static const std::string class_name = "Light";

    std::string getName() const;
    Port getPort() const;
    int getVal() const;
};

class Encoder: private Rotation, public ObjectTracker<Encoder>{
  private:
    std::string name;
    Port port;

  public:
    Encoder(Port port, std::string name);
    Encoder(Port port, std::string name, bool reverse);
    inline static const std::string class_name = "Encoder";
    inline static const c::v5_device_e device = c::E_DEVICE_ROTATION;

    std::string getName() const;
    Port getPort() const;
    int getVal();
    int getVel();
    std::int32_t reset();
    std::int32_t resetPos();
};

class _Distance: private Distance, public ObjectTracker<_Distance>{
  private:
    std::string name;
    Port port;

  public:
    _Distance(Port port, std::string name);
    inline static const std::string class_name = "Distance";
    inline static const c::v5_device_e device = c::E_DEVICE_DISTANCE;

    std::string getName() const;
    Port getPort() const;
    int getVal();
};

class Gyro: private Imu, public ObjectTracker<Gyro>{
  private:
    std::string name;
    Port port;

  public:
    Gyro(Port port, std::string name);
    inline static const std::string class_name = "Gyro";
    inline static const c::v5_device_e device = c::E_DEVICE_IMU;

    std::string getName() const;
    Port getPort() const;
    void tare();
    bool isCalibrating() const;
    double getVal() const;
};