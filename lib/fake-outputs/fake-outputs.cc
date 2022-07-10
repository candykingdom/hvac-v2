#include "fake-outputs.h"

bool FakeOutputs::Init() { return true; }

void FakeOutputs::SetFan(uint8_t value) {
  bool changed = value != fan_target_;
  Outputs::SetFan(value);
}

void FakeOutputs::SetPump(uint8_t value) {
  bool changed = value != pump_;
  Outputs::SetPump(value);
}