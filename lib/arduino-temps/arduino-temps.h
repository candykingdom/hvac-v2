#ifndef ARDUINO_TEMPS_H_
#define ARDUINO_TEMPS_H_

#include "temps.h"

class ArduinoTemps : public Temps {
 public:
  bool Init() override;
  float GetOutside() override;
  float GetInside() override;
};

#endif  // ARDUINO_TEMPS_H_