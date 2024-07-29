# 5225A-2022-2023

VEX Robotics Team 5255A's robot code for the 2022-2023 season: *Spin-up*

The robot performs odometry using 2 quadrature encoders in tandem with a gyroscope to track its position and orientation on a 12'x12' field. The odometry data is then fed into PID-controlled motion algorithms, achieving an accuracy of +/-2" and +/-1.5°.

`/main/src/Tracking.{hpp, cpp}`: Core odometry and autonomous motion algorithms

### Core files:
* `/main/src/main.cpp`: Contains the main driver control loop and autonomous period handler; entry point for program
* `/main/src/config.{hpp, cpp}`: Motor and sensor configuration
* `/main/src/Libraries/gui.{hpp, cpp}` **`(GUI Branch)`**: V5 screen GUI library using the PROS screen API
* `/main/src/Libraries/State.hpp`: Custom state machine library, improving on modularity with parameterized and asynchronous states
* `/main/src/logging.{hpp, cpp}`: SD card data logging
* `/main/src/queue.hpp`: Templated circular buffer class, used for logging and controller print queues
* `/main/src/auton.{hpp, cpp}`: Autonomous and programming skills routines
* `/main/src/controller.{hpp, cpp}`: Controller manager; controller printing queue and the global button handler
* `/main/src/drive.{hpp, cpp}`: Custom driver curve profiles and button action handlers
* `/main/src/Libraries/geometry.{hpp, cpp}`: Geometric math utilities and macros to aid in odometry and motion algorithms
* `/main/src/Libraries/pid.{hpp, cpp}`: Standardized PID algorithm handler library
* `/main/src/Libraries/task.{hpp, cpp}`: Multitasking manager
* `/main/src/Libraries/timer.{hpp, cpp}`: Timer library
* `/main/src/util.{hpp, cpp}`: Misc math and utility functions and macros

 `/main/src/Subsystems`: Folder containing state machine implementations for robot's flywheel, shooter, and intake subsystems
