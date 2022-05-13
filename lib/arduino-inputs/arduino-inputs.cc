#include "arduino-inputs.h"

#include <DS18B20.h>


bool ArduinoInputs::Init() {
  return true;
}

float ArduinoInputs::GetOutside() {
  return 0;
}

float ArduinoInputs::GetInside() {
  return 0;
}
