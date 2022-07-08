#include "arduino-inputs.h"

bool ArduinoInputs::Init() {
  pinMode(kWaterSwitchPin, INPUT_PULLUP);

  if (inside.getNumberOfDevices() < 1) {
    Serial.println("inputs: no inside temp sensor found");
    return false;
  }
  if (outside.getNumberOfDevices() < 1) {
    Serial.println("inputs: no outside temp sensor found");
    return false;
  }

  inside.selectNext();
  outside.selectNext();

  return true;
}

float ArduinoInputs::GetOutside() {
  if (outside.getNumberOfDevices() < 1) {
    return kNoTemp;
  }
  return outside.getTempF();
}

float ArduinoInputs::GetInside() {
  if (inside.getNumberOfDevices() < 1) {
    return kNoTemp;
  }
  return inside.getTempF();
}

bool ArduinoInputs::GetWaterSwitch() { return !digitalRead(kWaterSwitchPin); }