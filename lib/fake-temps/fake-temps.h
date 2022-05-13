#ifndef FAKE_TEMPS_H_
#define FAKE_TEMPS_H_

#include "temps.h"

class FakeTemps : public Temps {
 public:
  bool Init() override {}

  float GetOutside() override { return outside; }

  float GetInside() override { return inside; }

  float outside = 0;
  float inside = 0;
};

#endif  // FAKE_TEMPS_H_