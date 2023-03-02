#pragma once
#include "main.h"
#include "../config.hpp"
#include "../Libraries/state.hpp"
#include "intake.hpp"
#include "flywheel.hpp"

extern const int toaster_rpm;
extern const int barrier_rpm;

extern bool goal_disturb;

void shooterHandleInput();

// forward declarations
struct ShooterIdleParams;
struct ShooterShootParams;

extern bool angleOverride;

#define SHOOTER_STATE_TYPES ShooterIdleParams, ShooterShootParams

#define SHOOTER_STATE_TYPES_VARIANT std::variant<SHOOTER_STATE_TYPES>

extern Machine<SHOOTER_STATE_TYPES> shooter;

struct ShooterIdleParams{
  const char* getName();
  void handle();
  void handleStateChange(SHOOTER_STATE_TYPES_VARIANT prev_state);
};

struct ShooterShootParams{  
  ShooterShootParams(int shots = 3, bool match_load = false);

  const char* getName();
  void handle();
  void handleStateChange(SHOOTER_STATE_TYPES_VARIANT prev_state);

  int shots_left;
  bool disc_seen = false, disc_seen_last = false;
  bool match_load;

  FLYWHEEL_STATE_TYPES_VARIANT flywheel_state;
private:
  static Timer shoot_timer;

  static Timer disc_seen_timer;

  static Timer disc_absence_timer; // Ends match loads after not seeing for 2 seconds

  Timer cycle_check{"cycle_check"};
};



void shoot(int shots = 3, bool match_load = false);  // Shoots x number of shots

void handleRpm(); // Changes rpm based on number of crietria(angle_override, goal_disturb, piston_angle)
