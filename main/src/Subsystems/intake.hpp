#pragma once
#include "main.h"
#include "../config.hpp"
#include "../Libraries/state.hpp"

// forward declarations
struct IntakeIdleParams;
struct IntakeOnParams;
struct IntakeOffParams;
struct IntakeRevParams;
struct IntakeIndexParams;

#define INTAKE_STATE_TYPES IntakeIdleParams, IntakeOnParams, IntakeOffParams, IntakeRevParams, IntakeIndexParams

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
  int mag_ds_val;
  bool mag_disc_detected = false, mag_disc_detected_last = false;
  static const int mag_disc_thresh = 500;
};

void intakeOn(int8_t speed = 127);  // Wrapper function to turn intake on

struct IntakeOffParams{
  const char* getName();
  void handle();
  void handleStateChange(INTAKE_STATE_TYPES_VARIANT prev_state);
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