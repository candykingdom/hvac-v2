#ifndef ARDUINO_OUTPUTS_H_
#define ARDUINO_OUTPUTS_H_

#include "Arduino.h"
#include "outputs.h"

class ArduinoOutputs : public Outputs {
 public:
  bool Init() override;
  void SetFan(uint8_t value) override;
  void SetPump(uint8_t value) override;

 private:
#ifdef ARDUINO_TEENSYLC
  static const int kFanPin = 0;
  static const int kPumpPin = 1;
#else
  static const int kFanPin = PA9;
  static const int kPumpPin = PA10;
#endif
};

#endif  // ARDUINO_OUTPUTS_H_