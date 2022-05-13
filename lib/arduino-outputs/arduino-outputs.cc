#include "arduino-outputs.h"

#include <Arduino.h>

bool ArduinoOutputs::Init() {
  pinMode(kFanPin, OUTPUT);
  pinMode(kPumpPin, OUTPUT);

  return true;
}

void ArduinoOutputs::SetFan(uint8_t value) {
  Outputs::SetFan(value);
  analogWrite(kFanPin, value);
}

void ArduinoOutputs::SetPump(uint8_t value) { analogWrite(kPumpPin, value); }