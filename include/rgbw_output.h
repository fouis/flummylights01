#pragma once

#include <Arduino.h>
#include <FastLED.h>

#include "FastLED_RGBW.h"

bool rgbwOutputInit(uint8_t pin, uint16_t ledCount);
bool rgbwOutputShow(const CRGBW *leds, uint16_t count);