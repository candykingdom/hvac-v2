#ifndef OUTPUTS_H_
#define OUTPUTS_H_

#include <cstdint>

enum class FanType {
  MOSFET,
  BRIDGE,
  PWM,
};

class Outputs {
 public:
  virtual bool Init() = 0;

  virtual void SetFan(uint8_t value) { fan_ = value; }

  uint8_t GetFan() { return fan_; }

  virtual void SetFanDirection(bool direction) { fan_direction_ = direction; }

  virtual void SetPump(uint8_t value) { pump_ = value; }

  uint8_t GetPump() { return pump_; }

  virtual void SetLed(bool on) { led_ = on; }

  bool GetLed() { return led_; }

  // Updates MOSFET/Bridge outputs. Call at 50Hz * 256.
  virtual void Tick() = 0;

 protected:
  uint8_t fan_;
  bool fan_direction_ = true;
  uint8_t pump_;
  bool led_ = false;

  static constexpr FanType fan_type_ = FanType::BRIDGE;
};

#endif  // OUTPUTS_H_