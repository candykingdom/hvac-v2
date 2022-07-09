#pragma once

#include <cstdint>

#include "inputs.h"
#include "outputs.h"

struct RunnerParams {
  bool mode_auto = true;
  bool use_water_switch = true;
  int8_t set_temp = 60;
  int8_t swamp_threshold = 70;
  uint8_t fan_speed = 255;
  uint8_t pump_speed = 255;
};

class Runner {
  public:
  Runner(const RunnerParams& params, Inputs& inputs, Outputs& outputs);

  // Runs one cycle. Non-blocking. Call periodically.
  void Tick();

  private:
  const RunnerParams& params_;
  Inputs& inputs_;
  Outputs& outputs_;

  bool invalid_ = false;
};