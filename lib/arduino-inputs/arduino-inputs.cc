#include "arduino-inputs.h"

bool ArduinoInputs::Init() {
  pinMode(kWaterSwitchPin, INPUT_PULLUP);
  inside = DS18B20(kInsidePin);
  outside = DS18B20(kOutsidePin);

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
  return outside.getTempF();
}

float ArduinoInputs::GetInside() {
  return inside.getTempF();
}

bool ArduinoInputs::GetWaterSwitch() {
  return !digitalRead(kWaterSwitchPin);
}