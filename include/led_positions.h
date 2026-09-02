#ifndef LED_POSITIONS_H
#define LED_POSITIONS_H

#include <Arduino.h>

#include "flummylights.h"

// Integer coordinates of the physical LED arrangement.
// Coordinate ranges are x = 0..5, y = 0..5, z = 0..6.
struct LedPosition
{
    uint8_t x;
    uint8_t y;
    uint8_t z;
};

constexpr uint8_t LED_SPACE_SIZE_X = 6;
constexpr uint8_t LED_SPACE_SIZE_Y = 6;
constexpr uint8_t LED_SPACE_SIZE_Z = 7;

extern const LedPosition ledPositions[NUM_LEDS];

#endif
