#ifndef ARDUINO_INPUTS_H_
#define ARDUINO_INPUTS_H_

#include <DallasTemperature.h>
#include <exponential-moving-average-filter.h>
#include <median-filter.h>

#include "inputs.h"

class ArduinoInputs : public Inputs {
 public:
  ArduinoInputs();

  bool Init() override;
  float GetOutside() override;
  float GetInside() override;
  bool GetWaterSwitch() override;
  uint16_t GetBatteryMillivolts() override;

 private:
  static constexpr int kInsidePin = PB5;
  static constexpr int kOutsidePin = PB7;
  static constexpr int kWaterSwitchPin = PB10;
  static constexpr int kBatteryPin = PA0;

  OneWire insideBus = OneWire(kInsidePin);
  OneWire outsideBus = OneWire(kOutsidePin);
  DallasTemperature inside = DallasTemperature(&insideBus);
  DallasTemperature outside = DallasTemperature(&outsideBus);
  DeviceAddress insideAddress = {0};
  DeviceAddress outsideAddress = {0};
  int32_t prev_outside_ = 0;
  int32_t prev_inside_ = 0;

  MedianFilter<uint16_t, uint16_t, 5> battery_median_filter_;
  ExponentialMovingAverageFilter<uint16_t> battery_average_filter_;
  // val = (previous * 3 + current) / 4
  static constexpr uint8_t kBatteryFilterAlpha = 96;
};

#endif  // ARDUINO_INPUTS_H_