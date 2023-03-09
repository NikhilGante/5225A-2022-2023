#pragma once
#include "flywheel.hpp"

static constexpr int barrier_rpm = 1800;

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
    ShooterShootParams(int shots = 3, bool match_load = false);

    inline static const std::string name = "ShooterShoot";
    void handle();
    void handleStateChange(shooterVariant prev_state);

    int shots_left;
    bool disc_seen = false, disc_seen_last = false;
    bool match_load;

    flywheelVariant flywheel_state;

  private:
    static Timer shoot_timer;
    static Timer disc_seen_timer;
    static Timer disc_absence_timer; // Ends match loads after not seeing for 2 seconds
    static Timer cycle_check;
};


void shoot(int shots = 3, bool match_load = false);  // Shoots x number of shots

void handleRpm(); // Changes rpm based on number of crietria(angle_override, goal_disturb, piston_angle)
