#ifndef FAKE_INPUTS_H_
#define FAKE_INPUTS_H_

#include "inputs.h"

class FakeInputs : public Inputs {
 public:
  bool Init() override { return true; }

  float GetOutside() override { return outside; }

  float GetInside() override { return inside; }

  float outside = 0;
  float inside = 0;
};

#endif  // FAKE_INPUTS_H_