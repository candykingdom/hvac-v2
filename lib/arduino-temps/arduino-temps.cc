#include "arduino-temps.h"

#include <DS18B20.h>


bool ArduinoTemps::Init() {
  return true;
}

float ArduinoTemps::GetOutside() {
  return 0;
}

float ArduinoTemps::GetInside() {
  return 0;
}
