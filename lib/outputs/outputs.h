#ifndef OUTPUTS_H_
#define OUTPUTS_H_

#include <cstdint>

class Outputs {
 public:
  virtual bool Init();

  virtual void SetFan(uint8_t value) {
    fan_ = value;
  }

  uint8_t GetFan() {
    return fan_;
  }

  virtual void SetPump(uint8_t value) {
    pump_ = value;
  }

  uint8_t GetPump() {
    return pump_;
  }

  protected:
  uint8_t fan_;
  uint8_t pump_;
};

#endif  // OUTPUTS_H_