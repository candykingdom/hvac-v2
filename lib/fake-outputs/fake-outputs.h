#ifndef FAKE_OUTPUTS_H_
#define FAKE_OUTPUTS_H_

#include "outputs.h"

class FakeOutputs : public Outputs {
 public:
  FakeOutputs(FanType fan_type) : Outputs(fan_type) {}

  bool Init() override;
  void SetFan(uint8_t value) override;
  void SetPump(uint8_t value) override;
  bool GetFanDirection() { return fan_direction_; }
  uint8_t GetFanActual() { return fan_actual_; }
};

#endif  // FAKE_OUTPUTS_H_