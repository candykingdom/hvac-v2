#ifndef ARDUINO_INPUTS_H_
#define ARDUINO_INPUTS_H_

#include <DS18B20.h>
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

  DS18B20 inside = DS18B20(kInsidePin);
  DS18B20 outside = DS18B20(kOutsidePin);

  MedianFilter<uint16_t, uint16_t, 5> battery_median_filter_;
  ExponentialMovingAverageFilter<uint16_t> battery_average_filter_;
  // val = (previous * 3 + current) / 4
  static constexpr uint8_t kBatteryFilterAlpha = 96;
};

#endif  // ARDUINO_INPUTS_H_