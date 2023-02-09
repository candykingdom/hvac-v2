#include "arduino-inputs.h"

ArduinoInputs::ArduinoInputs() : battery_median_filter_(filter_functions::ForAnalogRead<kBatteryPin>()),
      battery_average_filter_(
          [this]() { return battery_median_filter_.GetFilteredValue(); },
          kBatteryFilterAlpha) {}

bool ArduinoInputs::Init() {
  pinMode(kWaterSwitchPin, INPUT_PULLUP);
  pinMode(kBatteryPin, INPUT);

  insideBus.reset_search();
  if (!insideBus.search(insideAddress)) {
    Serial.println("inputs: no inside temp sensor found");
    return false;
  }
  outsideBus.reset_search();
  if (!outsideBus.search(outsideAddress)) {
    Serial.println("inputs: no outside temp sensor found");
    return false;
  }
  inside.setResolution(9);
  outside.setResolution(9);
  inside.setWaitForConversion(false);
  outside.setWaitForConversion(false);
  inside.requestTemperaturesByAddress(insideAddress);
  outside.requestTemperaturesByAddress(outsideAddress);

  return true;
}

float ArduinoInputs::GetOutside() {
  if (!outside.isConnected(outsideAddress)) {
    return kNoTemp;
  }
  if (outside.isConversionComplete()) {
    // T * 128 = C
    // C * 1.8 + 32 = F
    // T / 128 * 1.8 + 32 = F
    prev_outside_ = outside.getTemp(outsideAddress) / (128 / 1.8) + 32;
    outside.requestTemperaturesByAddress(outsideAddress);
  }
  return prev_outside_;
}

float ArduinoInputs::GetInside() {
  if (!inside.isConnected(insideAddress)) {
    return kNoTemp;
  }
  if (inside.isConversionComplete()) {
    // T * 128 = C
    // C * 1.8 + 32 = F
    // T * 128 * 1.8 + 32 = F
    prev_inside_ = inside.getTemp(insideAddress) / (128 / 1.8) + 32;
    inside.requestTemperaturesByAddress(insideAddress);
  }
  return prev_inside_;
}

bool ArduinoInputs::GetWaterSwitch() { return !digitalRead(kWaterSwitchPin); }

uint16_t ArduinoInputs::GetBatteryMillivolts() {
  // Use 32 bits to avoid clipping during calculation
  uint32_t battery_raw = analogRead(kBatteryPin);
  // Voltage divider is 390k and 110k resistor, reference voltage is 3.3v, DAC
  // precision is 10 bits -> 1024
  return (uint16_t)(battery_raw * (390 + 110) * 3.3 * 1000 / 110 / 1024);
}