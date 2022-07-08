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
  static const int kFanPin = PA9;
  static const int kPumpPin = PA10;
};

#endif  // ARDUINO_OUTPUTS_H_