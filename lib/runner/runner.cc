#include "runner.h"

extern void Warning();

Runner::Runner(const RunnerParams& params, Inputs& inputs, Outputs& outputs)
    : params_(params), inputs_(inputs), outputs_(outputs) {}

void Runner::Tick() {
  if (params_.mode_auto) {
    bool missing_water = false;
    // TODO: how should temp control behave when a temp is invalid?
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

    if (inside > params_.set_temp) {
      if (outside > inside) {
        if (outside > params_.swamp_threshold) {
          if (params_.use_water_switch) {
            if (inputs_.GetWaterSwitch()) {
              outputs_.SetPump(params_.pump_speed);
              outputs_.SetFan(params_.fan_speed);
            } else {
              // Swamp is requested, but not available.
              outputs_.SetFan(0);
              outputs_.SetPump(0);
              missing_water = true;
            }
          } else {
            // No water switch
            outputs_.SetPump(params_.pump_speed);
            outputs_.SetFan(params_.fan_speed);
          }
        } else {
          // outside < swamp_threshold
          outputs_.SetFan(0);
          outputs_.SetPump(0);
        }
      } else {
        // outside < inside
        outputs_.SetFan(params_.fan_speed);
        if (outside > params_.swamp_threshold) {
          if (params_.use_water_switch) {
            if (inputs_.GetWaterSwitch()) {
              outputs_.SetPump(params_.pump_speed);
            } else {
              // Swamp is requested, but not available.
              missing_water = true;
              outputs_.SetPump(0);
            }
          } else {
            // No water switch
            outputs_.SetPump(params_.pump_speed);
          }
        }
      }
    } else {
      // inside < set_temp
      outputs_.SetFan(0);
      outputs_.SetPump(0);
    }

    invalid_ = new_invalid;

    if (params_.use_water_switch) {
      outputs_.SetLed(missing_water);
    }
  } else {
    // Manual mode
    if (params_.use_water_switch && !inputs_.GetWaterSwitch()) {
    } else {
      outputs_.SetFan(params_.fan_speed);
      outputs_.SetPump(params_.pump_speed);
    }
  }
}