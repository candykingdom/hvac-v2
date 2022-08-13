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
  static constexpr uint32_t kValid = 0xDEADBEEF;

  RunMode run_mode = RunMode::AUTO;
  bool use_water_switch = true;
  float temp_deadband = 2;
  int8_t set_temp = 60;
  int8_t swamp_threshold = 70;
  uint8_t vent_fan_speed = 255;
  uint8_t swamp_fan_speed = 255;
  bool vent_direction = true;
  bool swamp_direction = true;
  uint8_t pump_speed = 255;
  uint16_t pump_period = 60;
  uint8_t pump_duty = 255;
  bool sound_on = true;

  // This should always be last, so that if we add a field, we'll update the stored defaults
  uint32_t valid = kValid;

  void SetDefaults() {
    run_mode = RunMode::AUTO;
    use_water_switch = true;
    temp_deadband = 2;
    set_temp = 60;
    swamp_threshold = 70;
    vent_fan_speed = 255;
    swamp_fan_speed = 255;
    vent_direction = true;
    swamp_direction = true;
    pump_speed = 255;
    pump_period = 60;
    pump_duty = 255;
    sound_on = true;
    valid = kValid;
  }
};

class Runner {
 public:
  enum class OutputMode {
    VENT,
    SWAMP,
    OFF,
  };

  Runner(const RunnerParams& params, Inputs& inputs, Outputs& outputs);

  // Runs one cycle. Non-blocking. Call periodically.
  void Tick();

  // For testing.
  OutputMode GetOutputMode() { return output_mode_; }

 private:
  void RunAuto();
  void RunSwamp();
  void RunVent();
  void RunOff();

  void SetPump();

  const RunnerParams& params_;
  Inputs& inputs_;
  Outputs& outputs_;

  bool invalid_ = false;
  OutputMode output_mode_;
  bool prev_outside_warmer_ = false;
};