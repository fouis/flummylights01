#include "flummylights.h"
#include "generator.h"

class GlowSparkle02LightShow : public LightShow
{
  private:
    enum : uint16_t { FRAME_MS = 20, SPARKLE_PULSE_WIDTH = 14000 }; // pulse about 21% of a cycle
    enum : uint8_t { MAX_SPARKLES = 12 };

    float waveTimeFrequency;
    float waveSpaceFrequency;
    float sparkleTimeFrequency;
    int sparkleCount;

    uint16_t wavePhaseA;
    uint16_t wavePhaseB;
    uint16_t waveTimeStepA;
    uint16_t waveTimeStepB;
    uint16_t waveSpaceStepA;
    uint16_t waveSpaceStepB;

    uint8_t activeSparkleCount;
    uint8_t sparkleLed[MAX_SPARKLES];
    uint8_t sparkleHue[MAX_SPARKLES];
    uint16_t sparklePhase[MAX_SPARKLES];
    uint16_t sparklePhaseStep[MAX_SPARKLES];

    uint32_t hash32(uint32_t value) const
    {
      value ^= value >> 16;
      value *= 0x7feb352dUL;
      value ^= value >> 15;
      value *= 0x846ca68bUL;
      value ^= value >> 16;
      return value;
    }

    void configureSparkles()
    {
      activeSparkleCount = static_cast<uint8_t>(constrain(sparkleCount, 1, MAX_SPARKLES));

      for (uint8_t i = 0; i < activeSparkleCount; ++i)
      {
        const uint32_t h1 = hash32(0x51f15e5dUL + i * 0x9e3779b9UL + activeSparkleCount);
        const uint32_t h2 = hash32(h1 ^ 0xa53a9e37UL);

        // Divide the strip into cells and put one fixed sparkle into each cell.
        // This keeps the points distributed while preserving a random-looking layout.
        const uint16_t cellStart = (static_cast<uint16_t>(i) * NUM_LEDS) / activeSparkleCount;
        const uint16_t cellEnd = (static_cast<uint16_t>(i + 1) * NUM_LEDS) / activeSparkleCount;
        const uint16_t cellWidth = (cellEnd > cellStart) ? (cellEnd - cellStart) : 1;
        sparkleLed[i] = static_cast<uint8_t>(cellStart + (h1 % cellWidth));

        // Green through cyan to blue.
        sparkleHue[i] = static_cast<uint8_t>(96 + (h2 % 65));

        // Each point starts at a different phase and repeats at a slightly different rate.
        sparklePhase[i] = static_cast<uint16_t>(h1 >> 8);
        const float rateFactor = 0.80f + 0.40f * static_cast<float>(h2 & 0xFF) / 255.0f;
        sparklePhaseStep[i] = Generator::phaseStepFromHz(
          sparkleTimeFrequency * rateFactor,
          FRAME_MS
        );
      }
    }

  public:
    GlowSparkle02LightShow()
      : waveTimeFrequency(0.14f),
        waveSpaceFrequency(1.4f),
        sparkleTimeFrequency(0.22f),
        sparkleCount(6),
        wavePhaseA(0),
        wavePhaseB(21845),
        waveTimeStepA(0),
        waveTimeStepB(0),
        waveSpaceStepA(0),
        waveSpaceStepB(0),
        activeSparkleCount(0)
    {
      addPar(new FloatParameter("WAVETIME", &waveTimeFrequency, 0.14f));
      addPar(new FloatParameter("WAVESPACE", &waveSpaceFrequency, 1.4f));
      addPar(new FloatParameter("SPARKTIME", &sparkleTimeFrequency, 0.22f));
      addPar(new IntParameter("SPARKSPACE", &sparkleCount, 6));
    }

    const char * getShowName()
    {
      return "Glow Sparkle 02";
    }

    const char * getShowKey()
    {
      return "glospa02";
    }

    const char * getPageColor()
    {
      return "#6E2A12";
    }

    void initShow()
    {
      wavePhaseA = 0;
      wavePhaseB = 21845;
      parChanged();
    }

    void parChanged()
    {
      waveTimeStepA = Generator::phaseStepFromHz(waveTimeFrequency, FRAME_MS);
      waveTimeStepB = Generator::phaseStepFromHz(-waveTimeFrequency * 0.67f, FRAME_MS);

      waveSpaceStepA = Generator::phaseStepFromCycles(waveSpaceFrequency, NUM_LEDS);
      waveSpaceStepB = Generator::phaseStepFromCycles(-waveSpaceFrequency * 1.37f, NUM_LEDS);

      configureSparkles();
    }
 
    void lightLoop()
    {
      EVERY_N_MILLISECONDS(FRAME_MS)
      {
        for (int ledIndex = 0; ledIndex < NUM_LEDS; ++ledIndex)
        {
          const uint16_t phaseA = wavePhaseA + static_cast<uint16_t>(ledIndex) * waveSpaceStepA;
          const uint16_t phaseB = wavePhaseB + static_cast<uint16_t>(ledIndex) * waveSpaceStepB;

          const uint8_t waveA = static_cast<uint8_t>((static_cast<int32_t>(sin16(phaseA)) + 32768) >> 8);
          const uint8_t waveB = static_cast<uint8_t>((static_cast<int32_t>(sin16(phaseB)) + 32768) >> 8);
          const uint8_t wobble = static_cast<uint8_t>(
            (3U * static_cast<uint16_t>(waveA) + 2U * static_cast<uint16_t>(waveB)) / 5U
          );

          // Warm background: dark red through red-orange, with brightness breathing with the pattern.
          const uint8_t hue = static_cast<uint8_t>(2 + scale8(wobble, 28));
          const uint8_t value = static_cast<uint8_t>(95 + scale8(wobble, 125));
          leds[ledIndex] = CHSV(hue, 255, value);
        }

        for (uint8_t i = 0; i < activeSparkleCount; ++i)
        {
          const uint8_t intensity = Generator::trianglePulse8(
            sparklePhase[i],
            SPARKLE_PULSE_WIDTH
          );

          if (intensity > 0)
          {
            const CRGB sparkleColor = CHSV(sparkleHue[i], 255, intensity);
            CRGBW &pixel = leds[sparkleLed[i]];
            pixel.r = qadd8(pixel.r, sparkleColor.r);
            pixel.g = qadd8(pixel.g, sparkleColor.g);
            pixel.b = qadd8(pixel.b, sparkleColor.b);
          }

          sparklePhase[i] += sparklePhaseStep[i];
        }

        wavePhaseA += waveTimeStepA;
        wavePhaseB += waveTimeStepB;
      }
    }
};


lightShows.push_back(new GlowSparkle02LightShow);
