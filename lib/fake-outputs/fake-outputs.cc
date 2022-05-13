#include "fake-outputs.h"

#include <Arduino.h>

bool FakeOutputs::Init() {
  return true;
}

void FakeOutputs::SetFan(uint8_t value) { 
  Serial.print("\nSet fan to ");
  Serial.println(value);
 }

void FakeOutputs::SetPump(uint8_t value) {
  Serial.print("\nSet pump to ");
  Serial.println(value);
}