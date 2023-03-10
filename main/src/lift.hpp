// #pragma once
// #include "main.h"
// #include "Libraries/state.hpp"

// extern int lift_index;
// extern int lift_arr[5];

// // forward declarations
// struct LiftIdleParams;
// struct LiftResetParams;
// struct LiftMTTParams;

// #define LIFT_STATE_TYPES LiftMTTParams, LiftIdleParams, LiftResetParams

// #define LIFT_STATE_TYPES_VARIANT std::variant<LIFT_STATE_TYPES>

// extern Machine<LIFT_STATE_TYPES> lift;

// struct LiftMTTParams{
//   int target;
//   int max_power;

//   LiftMTTParams(int target, int max_power = 127);

//   const char* getName();
//   void handle();
//   void handleStateChange(LIFT_STATE_TYPES_VARIANT prev_state);
// };

// struct LiftIdleParams{
//   const char* getName();
//   void handle();
//   void handleStateChange(LIFT_STATE_TYPES_VARIANT prev_state);
// };

// struct LiftResetParams{
//   const char* getName();
//   void handle();
//   void handleStateChange(LIFT_STATE_TYPES_VARIANT prev_state);
// };
