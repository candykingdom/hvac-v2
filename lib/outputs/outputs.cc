#include "outputs.h"

#ifdef ARDUINO
#include <Arduino.h>
#else
extern uint32_t millis();
#endif

void Outputs::SetFan(uint8_t value) {
  fan_target_ = value;
}

void Outputs::Tick() {
  const uint32_t now = millis();
  if (now - fan_last_update_ >= kFanUpdateMs) {
    if (fan_target_ > fan_actual_) {
      fan_actual_++;
    } else if (fan_target_ < fan_actual_) {
      fan_actual_--;
    }
    fan_last_update_ = now;
  }
}