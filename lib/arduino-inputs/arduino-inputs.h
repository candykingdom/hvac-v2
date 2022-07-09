#ifndef ARDUINO_INPUTS_H_
#define ARDUINO_INPUTS_H_

#include <DS18B20.h>

#include "inputs.h"

class ArduinoInputs : public Inputs {
 public:
  bool Init() override;
  float GetOutside() override;
  float GetInside() override;
  bool GetWaterSwitch() override;

 private:
  static constexpr int kInsidePin = PB5;
  static constexpr int kOutsidePin = PB7;
  static constexpr int kWaterSwitchPin = PB10;

  DS18B20 inside = DS18B20(kInsidePin);
  DS18B20 outside = DS18B20(kOutsidePin);
};

#endif  // ARDUINO_INPUTS_H_