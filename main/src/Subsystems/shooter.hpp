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

using shooterVariant = std::variant<SHOOTER_STATE_TYPES>;

extern Machine<SHOOTER_STATE_TYPES> shooter;

struct ShooterIdleParams{
  inline static const std::string name = "ShooterIdle";
  void handle();
  void handleStateChange(shooterVariant prev_state);
};

struct ShooterShootParams{  
  ShooterShootParams(int shots = 3);

  inline static const std::string name = "ShooterIdle";
  void handle();
  void handleStateChange(shooterVariant prev_state);

  int shots_left;

  flywheelVariant flywheel_state;
private:
  Timer shoot_timer{"shoot_timer"};

};

void shoot(int shots = 3);  // Shoots x number of shots
