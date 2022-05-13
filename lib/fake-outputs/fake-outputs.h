#ifndef FAKE_OUTPUTS_H_
#define FAKE_OUTPUTS_H_

#include "outputs.h"

class FakeOutputs : public Outputs {
 public:
  bool Init() override;
  void SetFan(uint8_t value) override;
  void SetPump(uint8_t value) override;
};

#endif  // FAKE_OUTPUTS_H_