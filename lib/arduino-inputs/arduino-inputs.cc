#include "arduino-inputs.h"

ArduinoInputs::ArduinoInputs() : battery_median_filter_(filter_functions::ForAnalogRead<kBatteryPin>()),
      battery_average_filter_(
          [this]() { return battery_median_filter_.GetFilteredValue(); },
          kBatteryFilterAlpha) {}

bool ArduinoInputs::Init() {
  pinMode(kWaterSwitchPin, INPUT_PULLUP);
  pinMode(kBatteryPin, INPUT);

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

uint16_t ArduinoInputs::GetBatteryMillivolts() {
  // Use 32 bits to avoid clipping during calculation
  uint32_t battery_raw = analogRead(kBatteryPin);
  // Voltage divider is 390k and 110k resistor, reference voltage is 3.3v, DAC
  // precision is 10 bits -> 1024
  return (uint16_t)(battery_raw * (390 + 110) * 3.3 * 1000 / 110 / 1024);
}