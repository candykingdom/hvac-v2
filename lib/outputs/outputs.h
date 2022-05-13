#ifndef OUTPUTS_H_
#define OUTPUTS_H_

#include <cstdint>

class Outputs {
 public:
  virtual bool Init();
  virtual void SetFan(uint8_t value);
  virtual void SetPump(uint8_t value);
};

#endif  // OUTPUTS_H_