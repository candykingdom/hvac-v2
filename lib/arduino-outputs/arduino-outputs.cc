#include "arduino-outputs.h"

#include <Arduino.h>

bool ArduinoOutputs::Init() {
  analogWriteResolution(8);
  analogWriteFrequency(kMosfetPwmFrequency);

  pinMode(kLedPin, OUTPUT);
  pinMode(kFanPin, OUTPUT);
  pinMode(kPumpPin, OUTPUT);
  pinMode(kBridgeNPin, OUTPUT);
  pinMode(kBridgePPin, OUTPUT);
  pinMode(kBridgeEnablePin, OUTPUT);
  digitalWrite(kBridgeEnablePin, LOW);
  pinMode(kPwmSignalPin, OUTPUT);

  SetFanDirection(true);

  return true;
}

void ArduinoOutputs::SetFan(uint8_t value) {
  Outputs::SetFan(value);
}

void ArduinoOutputs::SetFanDirection(bool direction) {
  fan_direction_ = direction;
  if (fan_type_ == FanType::BRIDGE) {
    digitalWrite(kBridgeNPin, !fan_direction_);
    digitalWrite(kBridgePPin, fan_direction_);
  }
}

void ArduinoOutputs::SetLed(bool on) {
  led_ = on;
  digitalWrite(kLedPin, on);
}

void ArduinoOutputs::Tick() {
  Outputs::Tick();
  if (fan_type_ == FanType::PWM) {
    analogWrite(kPwmSignalPin, fan_actual_);
  }

  if (pwm_counter_ == 0) {
    if (fan_actual_ > 0) {
      switch (fan_type_) {
        case FanType::MOSFET:
          digitalWrite(kFanPin, HIGH);
          break;

        case FanType::BRIDGE:
          digitalWrite(kBridgeEnablePin, HIGH);
          break;

        case FanType::PWM:
          // Already handled in SetFan
          break;
      };
    }
    if (pump_ > 0) {
      digitalWrite(kPumpPin, HIGH);
    }
  }
  if (fan_actual_ != 255 && pwm_counter_ == fan_actual_ + 1) {
    switch (fan_type_) {
      case FanType::MOSFET:
        digitalWrite(kFanPin, LOW);
        break;

      case FanType::BRIDGE:
        digitalWrite(kBridgeEnablePin, LOW);
        break;

      case FanType::PWM:
        // Already handled in SetFan
        break;
    };
  }

  if (pump_ != 255 && pwm_counter_ == pump_ + 1) {
    digitalWrite(kPumpPin, LOW);
  }

  pwm_counter_++;
}