#ifndef FAKE_INPUTS_H_
#define FAKE_INPUTS_H_

#include "inputs.h"

class FakeInputs : public Inputs {
 public:
  bool Init() override { return true; }

  float GetOutside() override { return outside; }

  float GetInside() override { return inside; }

  bool GetWaterSwitch() override { return water_switch; }

  uint16_t GetBatteryMillivolts() override { return battery_millivolts; };

  float outside = 0;
  float inside = 0;
  bool water_switch = true;
  uint16_t battery_millivolts;
};

#endif  // FAKE_INPUTS_H_