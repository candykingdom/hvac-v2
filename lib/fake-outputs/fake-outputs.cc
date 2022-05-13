#include "fake-outputs.h"

#include <Arduino.h>

bool FakeOutputs::Init() { return true; }

void FakeOutputs::SetFan(uint8_t value) {
  bool changed = value != fan_;
  Outputs::SetFan(value);
  if (changed) {
    Serial.print("\nSet fan to ");
    Serial.println(value);
  }
}

void FakeOutputs::SetPump(uint8_t value) {
  bool changed = value != pump_;
  Outputs::SetPump(value);
  if (changed) {
    Serial.print("\nSet pump to ");
    Serial.println(value);
  }
}