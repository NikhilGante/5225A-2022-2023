#pragma once
#include "../Libraries/state.hpp"
#include "flywheel.hpp"

static constexpr int barrier_rpm = 1820;

void shooterHandleInput();

// forward declarations
struct ShooterIdleParams;
struct ShooterShootParams;

extern bool angleOverride;

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

    inline static const std::string name = "ShooterShoot";
    void handle();
    void handleStateChange(shooterVariant prev_state);

    int shots_left;

    flywheelVariant flywheel_state;

  private:
    static Timer shoot_timer;

    Timer cycle_check{"cycle_check"};
};

void shoot(int shots = 3);  // Shoots x number of shots
