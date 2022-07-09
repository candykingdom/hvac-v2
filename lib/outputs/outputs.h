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
  virtual bool Init();

  virtual void SetFan(uint8_t value) { fan_ = value; }

  uint8_t GetFan() { return fan_; }

  virtual void SetFanDirection(bool direction) { fan_direction_ = direction; }

  virtual void SetPump(uint8_t value) { pump_ = value; }

  uint8_t GetPump() { return pump_; }

  // Updates MOSFET/Bridge outputs. Call at 50Hz.
  virtual void Tick() = 0;

 protected:
  uint8_t fan_;
  bool fan_direction_;
  uint8_t pump_;

  static constexpr FanType fan_type_ = FanType::BRIDGE;
};

#endif  // OUTPUTS_H_