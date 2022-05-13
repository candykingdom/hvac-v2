#ifndef ARDUINO_INPUTS_H_
#define ARDUINO_INPUTS_H_

#include "inputs.h"

class ArduinoInputs : public Inputs {
 public:
  bool Init() override;
  float GetOutside() override;
  float GetInside() override;
};

#endif  // ARDUINO_INPUTS_H_