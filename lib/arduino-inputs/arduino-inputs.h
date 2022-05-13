#ifndef ARDUINO_INPUTS_H_
#define ARDUINO_INPUTS_H_

#include "inputs.h"
#include <DS18B20.h>

class ArduinoInputs : public Inputs {
 public:
  bool Init() override;
  float GetOutside() override;
  float GetInside() override;
  bool GetWaterSwitch() override;

 private:
#ifdef ARDUINO_TEENSYLC
  static const int kWaterSwitchPin = 2;
  static const uint8_t kInsidePin = 6;
  static const uint8_t kOutsidePin = 7;
#else
  static const int kInsidePin = PB6;
  static const int kOutsidePin = PB7;
  static const int kWaterSwitchPin = PB10;
#endif

 DS18B20 inside;
 DS18B20 outside;

};

#endif  // ARDUINO_INPUTS_H_