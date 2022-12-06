#pragma once
#include "main.h"
#include "../config.hpp"
#include "../Libraries/state.hpp"
#include "intake.hpp"
#include "flywheel.hpp"

void shooterHandleInput();

// forward declarations
struct ShooterIdleParams;
struct ShooterShootParams;

#define SHOOTER_STATE_TYPES ShooterIdleParams, ShooterShootParams

#define SHOOTER_STATE_TYPES_VARIANT std::variant<SHOOTER_STATE_TYPES>

extern Machine<SHOOTER_STATE_TYPES> shooter;

struct ShooterIdleParams{
  const char* getName();
  void handle();
  void handleStateChange(SHOOTER_STATE_TYPES_VARIANT prev_state);
};

struct ShooterShootParams{  
  ShooterShootParams(int shots = 3);

  const char* getName();
  void handle();
  void handleStateChange(SHOOTER_STATE_TYPES_VARIANT prev_state);

  int shots_left;

  FLYWHEEL_STATE_TYPES_VARIANT flywheel_state;
private:
  Timer shoot_timer{"shoot_timer"};

};

void shoot(int shots = 3);  // Shoots x number of shots
