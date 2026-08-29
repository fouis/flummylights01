#ifndef GENERATOR_H
#define GENERATOR_H

#include <Arduino.h>
#include <FastLED.h>

class SineGenerator
{
  public:
    // Construct a generator with a defined, harmless default configuration.
    SineGenerator();

    // Reset phase while keeping frequency, amplitude and base time step.
    void init();
    void reset();

    // Configure and reset the generator.
    void init(uint16_t myBaseTimeStep, float myGenFrequency, int32_t myAmplitude);

    void setFrequency(float myGenFrequency);
    void setAmplitude(int32_t myAmplitude);

    // Advance one time step and return the scaled signed sine value.
    int32_t step();

  private:
    uint16_t phase;
    uint16_t genTimeStep;
    uint16_t baseTimeStep;
    float genFrequency;
    int32_t amplitude;
};

namespace Generator
{
    // Convert a frequency in Hz and a frame duration in milliseconds to a
    // FastLED-style 16-bit phase increment. Negative frequencies are allowed
    // and naturally run in the opposite direction through uint16_t wrapping.
    uint16_t phaseStepFromHz(float frequencyHz, uint16_t frameTimeMs);

    // Convert a spatial frequency in cycles across a number of samples (LEDs)
    // to the corresponding 16-bit phase increment per sample.
    uint16_t phaseStepFromCycles(float cycles, uint16_t sampleCount);

    // Linear pulse envelope. During pulseWidth phase units the result rises
    // linearly 0 -> 255 and falls linearly 255 -> 0; it is zero for the rest
    // of the 16-bit cycle. Useful for periodic sparkle flashes.
    uint8_t trianglePulse8(uint16_t phase, uint16_t pulseWidth);
}

#endif
