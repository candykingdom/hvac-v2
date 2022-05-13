#include <DS18B20.h>

namespace temps {

void Init() {

}

float GetOutside() {
  return 40 + random(80);
}

float GetInside() {
  return 50 + random(50);
}

}  // namespace temps