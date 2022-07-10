#pragma once

#include <cstdint>

enum class FanType {
  MOSFET,
  BRIDGE,
  PWM,
};

class Outputs {
 public:
  Outputs(FanType fan_type) : fan_type_(fan_type) {}

  virtual bool Init() = 0;

  virtual void SetFan(uint8_t value);

  uint8_t GetFan() { return fan_target_; }

  virtual void SetFanDirection(bool direction) { fan_direction_ = direction; }

  virtual void SetPump(uint8_t value) { pump_ = value; }

  uint8_t GetPump() { return pump_; }

  virtual void SetLed(bool on) { led_ = on; }

  bool GetLed() { return led_; }

  // Updates MOSFET/Bridge outputs. Call at 50Hz * 256.
  virtual void Tick();

  static constexpr uint32_t kFanUpdateMs = 1;

 protected:
  uint8_t fan_target_ = 0;
  uint8_t fan_actual_  = 0;
  uint32_t fan_last_update_ = 0;

  bool fan_direction_ = true;
  uint8_t pump_ = 0;
  bool led_ = false;

  const FanType fan_type_;
};
