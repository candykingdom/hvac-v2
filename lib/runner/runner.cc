#include "runner.h"

extern void Warning();

Runner::Runner(const RunnerParams& params, Inputs& inputs, Outputs& outputs)
    : params_(params), inputs_(inputs), outputs_(outputs) {}

void Runner::Tick() {
  switch (params_.run_mode) {
    case RunMode::AUTO:
      RunAuto();
      break;

    case RunMode::SWAMP:
      RunSwamp();
      break;

    case RunMode::VENT:
      RunVent();
      break;

    case RunMode::OFF:
      RunOff();
      break;
  }
}

void Runner::RunAuto() {
  bool missing_water = false;
  bool new_invalid = false;
  float inside = inputs_.GetInside();
  if (inside <= Inputs::kNoTemp) {
    if (!invalid_) {
      Warning();
    }
    invalid_ = true;
    new_invalid = true;
  }
  float outside = inputs_.GetOutside();
  if (outside <= Inputs::kNoTemp) {
    if (!invalid_) {
      Warning();
    }
    invalid_ = true;
    new_invalid = true;
  }
  if (invalid_) {
    outputs_.SetFan(0);
    outputs_.SetPump(0);
    return;
  }

  if (inside > params_.set_temp) {
    if (outside > inside) {
      if (outside > params_.swamp_threshold) {
        if (params_.use_water_switch) {
          if (inputs_.GetWaterSwitch()) {
            output_mode_ = OutputMode::SWAMP;
          } else {
            // Swamp is requested, but not available.
            output_mode_ = OutputMode::OFF;
            missing_water = true;
          }
        } else {
          // No water switch
          output_mode_ = OutputMode::SWAMP;
        }
      } else {
        // outside < swamp_threshold
        output_mode_ = OutputMode::OFF;
      }
    } else {
      // outside < inside
      if (outside > params_.swamp_threshold) {
        if (params_.use_water_switch) {
          if (inputs_.GetWaterSwitch()) {
            output_mode_ = OutputMode::SWAMP;
          } else {
            // Swamp is requested, but not available.
            missing_water = true;
            output_mode_ = OutputMode::VENT;
          }
        } else {
          // No water switch
          output_mode_ = OutputMode::SWAMP;
        }
      } else {
        // outside < params.swamp_threshold
        output_mode_ = OutputMode::VENT;
      }
    }
  } else {
    // inside < set_temp
    output_mode_ = OutputMode::OFF;
  }

  switch (output_mode_) {
    case OutputMode::VENT:
      outputs_.SetFanDirection(params_.vent_direction);
      outputs_.SetFan(params_.vent_fan_speed);
      outputs_.SetPump(0);
      break;

    case OutputMode::SWAMP:
      outputs_.SetFanDirection(params_.swamp_direction);
      outputs_.SetFan(params_.swamp_fan_speed);
      outputs_.SetPump(params_.pump_speed);
      break;

    case OutputMode::OFF:
      outputs_.SetFan(0);
      outputs_.SetPump(0);
      break;
  }

  invalid_ = new_invalid;

  if (params_.use_water_switch) {
    outputs_.SetLed(missing_water);
  }
}

void Runner::RunSwamp() {
  if (params_.pump_speed == 0) {
    outputs_.SetLed(false);
    outputs_.SetFan(0);
    outputs_.SetPump(0);
  } else {
    if (params_.use_water_switch && !inputs_.GetWaterSwitch()) {
      outputs_.SetLed(true);
      outputs_.SetFan(0);
      outputs_.SetPump(0);
    } else {
      outputs_.SetLed(false);
      outputs_.SetFan(params_.swamp_fan_speed);
      outputs_.SetPump(params_.pump_speed);
      outputs_.SetFanDirection(params_.swamp_direction);
    }
  }
}

void Runner::RunVent() {
  outputs_.SetLed(false);
  outputs_.SetFan(params_.vent_fan_speed);
  outputs_.SetFanDirection(params_.vent_direction);
  outputs_.SetPump(0);
}

void Runner::RunOff() {
  outputs_.SetFan(0);
  outputs_.SetPump(0);
}