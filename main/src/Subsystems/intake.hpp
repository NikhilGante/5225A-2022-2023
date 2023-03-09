#pragma once
#include "../Libraries/state.hpp"

void intakeHandleInput();

extern std::atomic<int> g_mag_disc_count;

// forward declarations
struct IntakeIdleParams;
struct IntakeOnParams;
struct IntakeOffParams;
struct IntakeRevParams;
struct IntakeIndexParams;
struct IntakeRollerParams;

#define INTAKE_STATE_TYPES IntakeIdleParams, IntakeOnParams, IntakeOffParams, IntakeRevParams, IntakeIndexParams, IntakeRollerParams

using intakeVariant = std::variant<INTAKE_STATE_TYPES>;

extern Machine<INTAKE_STATE_TYPES> intake;

struct IntakeIdleParams{
  inline static const std::string name = "IntakeIdle";

  void handle();
  void handleStateChange(intakeVariant prev_state);
};

struct IntakeOnParams{
  int8_t speed = 127;

  IntakeOnParams(int8_t speed = 127);
  
  inline static const std::string name = "IntakeOn";
  void handle();
  void handleStateChange(intakeVariant prev_state);

private:
  int mag_ds_val;
  bool mag_disc_detected = false, mag_disc_detected_last = false;
  static constexpr int mag_disc_thresh = 1000;
};

void intakeOn(int8_t speed = 127);  // Wrapper function to turn intake on

struct IntakeOffParams{
  inline static const std::string name = "IntakeOff";
  void handle();
  void handleStateChange(intakeVariant prev_state);
};

void intakeOff();  // Wrapper function to turn intake off

struct IntakeRevParams{
  int8_t speed = -127;

  IntakeRevParams(int8_t speed = -127);

  inline static const std::string name = "IntakeRev";
  void handle();
  void handleStateChange(intakeVariant prev_state);
};

void intakeRev(int8_t speed = -127);  // Wrapper function to turn intake in reverse

struct IntakeIndexParams{
  int8_t speed = -127;

  IntakeIndexParams(int8_t speed = -127);

  inline static const std::string name = "IntakeIndex";
  void handle();
  void handleStateChange(intakeVariant prev_state);
};

void intakeIndex(int8_t speed = -127);  // Wrapper function to make intake index discs

struct IntakeRollerParams{
  double degrees; // How many degrees to spin the roller
  IntakeRollerParams(double degrees = 300); // 300 for auto, 500 for skills

  inline static const std::string name = "IntakeRoller";
  void handle();
  void handleStateChange(intakeVariant prev_state);
};

void spinRoller(double degrees = 300);  // Wrapper function to make intake index discs