#pragma once

#include <cstdint>

#include "inputs.h"
#include "outputs.h"

struct RunnerParams {
  bool mode_auto;
  bool use_water_switch;
  int8_t set_temp;
  int8_t swamp_threshold;
  uint8_t fan_speed;
  uint8_t pump_speed;
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