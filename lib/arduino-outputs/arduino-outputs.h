#ifndef ARDUINO_OUTPUTS_H_
#define ARDUINO_OUTPUTS_H_

#include "Arduino.h"
#include "outputs.h"

class ArduinoOutputs : public Outputs {
 public:
  bool Init() override;
  void SetFan(uint8_t value) override;
  void SetFanDirection(bool direction) override;
  void Tick() override;

 private:
  uint8_t pwm_counter_ = 0;
  static constexpr int kFanPin = PA9;
  static constexpr int kPumpPin = PA10;
  static constexpr int kBridgeNPin = PB2;
  static constexpr int kBridgePPin = PB3;
  static constexpr int kBridgeEnablePin = PB0;
  static constexpr int kPwmSignalPin = PA8;

  // Note: don't make this too high, or it might fry the board!
  static constexpr uint32_t kMosfetPwmFrequency = 50;
  static constexpr uint32_t kSignalPwmFrequency = 30000;
};

#endif  // ARDUINO_OUTPUTS_H_