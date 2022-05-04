#include <DS18B20.h>

namespace temps {

void Init() {

}

float GetOutside() {
  return 40 + random(80);
}

}  // namespace temps