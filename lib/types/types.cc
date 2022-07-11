#include "types.h"

#ifndef ARDUINO
uint32_t current_time_ = 0;

uint32_t millis() { return current_time_; }
void SetMillis(uint32_t millis) { current_time_ = millis; }
void AdvanceMillis(uint32_t millis) { current_time_ += millis; }
#endif  // ARDUINO