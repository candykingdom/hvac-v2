#pragma once

#include <cstdint>

#include "inputs.h"
#include "outputs.h"

enum class RunMode {
  AUTO,
  SWAMP,
  VENT,
  OFF,
};

struct RunnerParams {
  RunMode run_mode = RunMode::AUTO;
  bool use_water_switch = true;
  int8_t set_temp = 60;
  int8_t swamp_threshold = 70;
  uint8_t vent_fan_speed = 255;
  uint8_t swamp_fan_speed = 255;
  bool vent_direction = true;
  bool swamp_direction = true;
  uint8_t pump_speed = 255;
};

class Runner {
 public:
  Runner(const RunnerParams& params, Inputs& inputs, Outputs& outputs);

  // Runs one cycle. Non-blocking. Call periodically.
  void Tick();

 private:
  void RunAuto();
  void RunSwamp();
  void RunVent();
  void RunOff();

  enum class OutputMode {
    VENT,
    SWAMP,
    OFF,
  };

  const RunnerParams& params_;
  Inputs& inputs_;
  Outputs& outputs_;

  bool invalid_ = false;
  OutputMode output_mode_;
};