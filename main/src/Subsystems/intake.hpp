#pragma once
#include "main.h"
#include "../config.hpp"
#include "../Libraries/state.hpp"

#define MAG_DS_THRESH 1500
#define UPTK_DS_THRESH 1500

void intakeHandleInput();

extern Timer intk_off_buzz_timer;

extern atomic<int> g_mag_disc_count;

// forward declarations
struct IntakeIdleParams;
struct IntakeOnParams;
struct IntakeOffParams;
struct IntakeRevParams;
struct IntakeIndexParams;
struct IntakeRollerParams;

#define INTAKE_STATE_TYPES IntakeIdleParams, IntakeOnParams, IntakeOffParams, IntakeRevParams, IntakeIndexParams, IntakeRollerParams

#define INTAKE_STATE_TYPES_VARIANT std::variant<INTAKE_STATE_TYPES>

extern Machine<INTAKE_STATE_TYPES> intake;

struct IntakeIdleParams{
  const char* getName();
  void handle();
  void handleStateChange(INTAKE_STATE_TYPES_VARIANT prev_state);
};

struct IntakeOnParams{
  int8_t speed = 127;

  IntakeOnParams(int8_t speed = 127);
  
  const char* getName();
  void handle();
  void handleStateChange(INTAKE_STATE_TYPES_VARIANT prev_state);

private:
  int mag_ds_val, jam_cycle = 0;
  bool mag_disc_detected = false, mag_disc_detected_last = false;
  bool disc_override = true;
};

void intakeOn(int8_t speed = 127);  // Wrapper function to turn intake on

struct IntakeOffParams{
  const char* getName();
  void handle();
  void handleStateChange(INTAKE_STATE_TYPES_VARIANT prev_state);
private:
  Timer possession_notify_timer{"possession_notify_timer"};
};

void intakeOff();  // Wrapper function to turn intake off

struct IntakeRevParams{
  int8_t speed = -127;

  IntakeRevParams(int8_t speed = -127);

  const char* getName();
  void handle();
  void handleStateChange(INTAKE_STATE_TYPES_VARIANT prev_state);
};

void intakeRev(int8_t speed = -127);  // Wrapper function to turn intake in reverse

struct IntakeIndexParams{
  int8_t speed = -127;

  IntakeIndexParams(int8_t speed = -127);

  const char* getName();
  void handle();
  void handleStateChange(INTAKE_STATE_TYPES_VARIANT prev_state);
};

void intakeIndex(int8_t speed = -127);  // Wrapper function to make intake index discs

struct IntakeRollerParams{
  IntakeRollerParams(); // 300 for auto, 500 for skills

  const char* getName();
  void handle();
  void handleStateChange(INTAKE_STATE_TYPES_VARIANT prev_state);
};

void rollerOpControl();  // Wrapper function to make intake index discs

bool diskInIntake();