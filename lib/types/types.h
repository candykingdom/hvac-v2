#pragma once

#include <cstdint>

#ifdef ARDUINO
#include <Arduino.h>
#else
static uint32_t millis_ = 0;
uint32_t millis();

void SetMillis(uint32_t millis);
void AdvanceMillis(uint32_t millis);
#endif
