#include "generator.h"

#include <math.h>


SineGenerator::SineGenerator()
  : phase(0),
    genTimeStep(0),
    baseTimeStep(1),
    genFrequency(0.0f),
    amplitude(0)
{
}

void SineGenerator::init()
{
    reset();
}

void SineGenerator::reset()
{
    phase = 0;
}

void SineGenerator::init(uint16_t myBaseTimeStep, float myGenFrequency, int32_t myAmplitude)
{
    baseTimeStep = myBaseTimeStep;
    amplitude = myAmplitude;
    setFrequency(myGenFrequency);
    reset();
}

int32_t SineGenerator::step()
{
    const int32_t value = amplitude * static_cast<int32_t>(sin16(phase));
    phase += genTimeStep;
    return value;
}

void SineGenerator::setFrequency(float myGenFrequency)
{
    genFrequency = myGenFrequency;
    genTimeStep = static_cast<uint16_t>(lroundf(65.536f * baseTimeStep * genFrequency));
}

void SineGenerator::setAmplitude(int32_t myAmplitude)
{
    amplitude = myAmplitude;
}

namespace Generator
{
uint16_t phaseStepFromHz(float frequencyHz, uint16_t frameTimeMs)
{
    const float phaseStep = 65.536f * static_cast<float>(frameTimeMs) * frequencyHz;
    return static_cast<uint16_t>(lroundf(phaseStep));
}

uint16_t phaseStepFromCycles(float cycles, uint16_t sampleCount)
{
    if (sampleCount == 0)
    {
        return 0;
    }

    const float phaseStep = 65536.0f * cycles / static_cast<float>(sampleCount);
    return static_cast<uint16_t>(lroundf(phaseStep));
}

uint8_t trianglePulse8(uint16_t phase, uint16_t pulseWidth)
{
    if (pulseWidth < 2 || phase >= pulseWidth)
    {
        return 0;
    }

    const uint16_t riseWidth = pulseWidth / 2;
    const uint16_t fallWidth = pulseWidth - riseWidth;

    if (phase <= riseWidth)
    {
        return static_cast<uint8_t>((static_cast<uint32_t>(phase) * 255U) / riseWidth);
    }

    return static_cast<uint8_t>(
        (static_cast<uint32_t>(pulseWidth - phase) * 255U) / fallWidth
    );
}
}
