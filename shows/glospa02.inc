#include "flummylights.h"
#include "generator.h"

class GlowSparkle02LightShow : public LightShow
{
  private:
    enum : uint16_t { FRAME_MS = 20, SPARKLE_FLASH_MS = 140 };
    enum : uint8_t { MAX_SPARKLES = 12 };

    float waveTimeFrequency;
    float waveSpaceFrequency;
    float sparkleTimeFrequency;
    int sparkleCount;

    uint16_t brightnessPhaseA;
    uint16_t brightnessPhaseB;
    uint16_t colorPhaseA;
    uint16_t colorPhaseB;

    uint16_t brightnessTimeStepA;
    uint16_t brightnessTimeStepB;
    uint16_t brightnessSpaceStepA;
    uint16_t brightnessSpaceStepB;
    uint16_t colorTimeStepA;
    uint16_t colorTimeStepB;
    uint16_t colorSpaceStepA;
    uint16_t colorSpaceStepB;

    uint8_t activeSparkleCount;
    uint8_t sparkleLed[MAX_SPARKLES];
    uint8_t sparkleHue[MAX_SPARKLES];
    bool sparkleActive[MAX_SPARKLES];
    uint32_t sparkleStartedAt[MAX_SPARKLES];
    uint32_t nextSparkleAt[MAX_SPARKLES];

    uint8_t sine8(uint16_t phase) const
    {
      return static_cast<uint8_t>(
        (static_cast<int32_t>(sin16(phase)) + 32768) >> 8
      );
    }

    uint32_t sparkleIntervalMs() const
    {
      const float frequency = max(sparkleTimeFrequency, 0.01f);
      return static_cast<uint32_t>(1000.0f / frequency);
    }

    uint32_t randomizedSparkleIntervalMs() const
    {
      const uint32_t baseInterval = sparkleIntervalMs();
      const uint32_t jitterPercent = static_cast<uint32_t>(random(80, 121));
      return (baseInterval * jitterPercent) / 100U;
    }

    void chooseSparkle(uint8_t index)
    {
      const uint16_t cellStart =
        (static_cast<uint16_t>(index) * NUM_LEDS) / activeSparkleCount;
      const uint16_t cellEnd =
        (static_cast<uint16_t>(index + 1) * NUM_LEDS) / activeSparkleCount;

      const uint16_t safeCellEnd = (cellEnd > cellStart) ? cellEnd : (cellStart + 1);

      // Each group owns a section of the strip, but each flash chooses a new
      // random LED inside that section.
      sparkleLed[index] = static_cast<uint8_t>(random(cellStart, safeCellEnd));
      sparkleHue[index] = static_cast<uint8_t>(random(96, 161)); // green -> blue
    }

    void configureSparkles()
    {
      activeSparkleCount = static_cast<uint8_t>(
        constrain(sparkleCount, 1, MAX_SPARKLES)
      );

      const uint32_t now = millis();
      const uint32_t baseInterval = sparkleIntervalMs();
      const uint32_t groupSpacing = max(
        1UL,
        baseInterval / static_cast<uint32_t>(activeSparkleCount)
      );

      for (uint8_t i = 0; i < MAX_SPARKLES; ++i)
      {
        sparkleActive[i] = false;
        sparkleStartedAt[i] = 0;

        if (i < activeSparkleCount)
        {
          chooseSparkle(i);

          // Start the groups distributed over roughly one base period, with
          // some additional jitter so the pattern does not look clocked.
          nextSparkleAt[i] = now
            + static_cast<uint32_t>(i) * groupSpacing
            + static_cast<uint32_t>(random(0, groupSpacing + 1));
        }
        else
        {
          nextSparkleAt[i] = 0;
        }
      }
    }

    uint8_t sparkleIntensity(uint32_t elapsedMs) const
    {
      if (elapsedMs >= SPARKLE_FLASH_MS)
      {
        return 0;
      }

      const uint32_t halfFlash = SPARKLE_FLASH_MS / 2U;

      if (elapsedMs <= halfFlash)
      {
        return static_cast<uint8_t>((elapsedMs * 255U) / halfFlash);
      }

      return static_cast<uint8_t>(
        ((SPARKLE_FLASH_MS - elapsedMs) * 255U) / halfFlash
      );
    }

  public:
    GlowSparkle02LightShow()
      : waveTimeFrequency(0.42f),
        waveSpaceFrequency(3.30f),
        sparkleTimeFrequency(0.54f),
        sparkleCount(4),
        brightnessPhaseA(0),
        brightnessPhaseB(21845),
        colorPhaseA(10923),
        colorPhaseB(43690),
        brightnessTimeStepA(0),
        brightnessTimeStepB(0),
        brightnessSpaceStepA(0),
        brightnessSpaceStepB(0),
        colorTimeStepA(0),
        colorTimeStepB(0),
        colorSpaceStepA(0),
        colorSpaceStepB(0),
        activeSparkleCount(0)
    {
      addPar(new FloatParameter("WAVETIME", &waveTimeFrequency, 0.42f));
      addPar(new FloatParameter("WAVESPACE", &waveSpaceFrequency, 3.30f));
      addPar(new FloatParameter("SPARKTIME", &sparkleTimeFrequency, 0.54f));
      addPar(new IntParameter("SPARKSPACE", &sparkleCount, 4));
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
      brightnessPhaseA = 0;
      brightnessPhaseB = 21845;
      colorPhaseA = 10923;
      colorPhaseB = 43690;
      parChanged();
    }

    void parChanged()
    {
      // Brightness moves comparatively slowly.
      brightnessTimeStepA = Generator::phaseStepFromHz(
        waveTimeFrequency,
        FRAME_MS
      );
      brightnessTimeStepB = Generator::phaseStepFromHz(
        -waveTimeFrequency * 0.61f,
        FRAME_MS
      );
      brightnessSpaceStepA = Generator::phaseStepFromCycles(
        waveSpaceFrequency,
        NUM_LEDS
      );
      brightnessSpaceStepB = Generator::phaseStepFromCycles(
        -waveSpaceFrequency * 1.29f,
        NUM_LEDS
      );

      // Color uses the same user parameters as a base, but intentionally runs
      // somewhat faster and at different spatial frequencies.
      colorTimeStepA = Generator::phaseStepFromHz(
        waveTimeFrequency * 1.35f,
        FRAME_MS
      );
      colorTimeStepB = Generator::phaseStepFromHz(
        -waveTimeFrequency * 0.91f,
        FRAME_MS
      );
      colorSpaceStepA = Generator::phaseStepFromCycles(
        waveSpaceFrequency * 1.32f,
        NUM_LEDS
      );
      colorSpaceStepB = Generator::phaseStepFromCycles(
        -waveSpaceFrequency * 1.73f,
        NUM_LEDS
      );

      configureSparkles();
    }
 
    void lightLoop()
    {
      EVERY_N_MILLISECONDS(FRAME_MS)
      {
        for (int ledIndex = 0; ledIndex < NUM_LEDS; ++ledIndex)
        {
          const uint16_t brightnessLedPhaseA = brightnessPhaseA
            + static_cast<uint16_t>(ledIndex) * brightnessSpaceStepA;
          const uint16_t brightnessLedPhaseB = brightnessPhaseB
            + static_cast<uint16_t>(ledIndex) * brightnessSpaceStepB;
          const uint16_t colorLedPhaseA = colorPhaseA
            + static_cast<uint16_t>(ledIndex) * colorSpaceStepA;
          const uint16_t colorLedPhaseB = colorPhaseB
            + static_cast<uint16_t>(ledIndex) * colorSpaceStepB;

          const uint8_t brightnessWaveA = sine8(brightnessLedPhaseA);
          const uint8_t brightnessWaveB = sine8(brightnessLedPhaseB);
          const uint8_t colorWaveA = sine8(colorLedPhaseA);
          const uint8_t colorWaveB = sine8(colorLedPhaseB);

          const uint8_t brightnessWobble = static_cast<uint8_t>(
            (3U * static_cast<uint16_t>(brightnessWaveA)
             + 2U * static_cast<uint16_t>(brightnessWaveB)) / 5U
          );
          const uint8_t colorWobble = static_cast<uint8_t>(
            (2U * static_cast<uint16_t>(colorWaveA)
             + 3U * static_cast<uint16_t>(colorWaveB)) / 5U
          );

          // Independent brightness and color modulation. The direct RGB mix
          // keeps the dark red -> orange transition smooth at low intensity.
          const uint8_t brightness = static_cast<uint8_t>(
            80 + scale8(brightnessWobble, 160)
          );
          const uint8_t warmRed = static_cast<uint8_t>(
            215 + scale8(colorWobble, 40)
          );
          const uint8_t warmGreen = static_cast<uint8_t>(
            10 + scale8(colorWobble, 75)
          );

          leds[ledIndex].r = scale8(warmRed, brightness);
          leds[ledIndex].g = scale8(warmGreen, brightness);
          leds[ledIndex].b = 0;
          leds[ledIndex].w = 0;
        }

        const uint32_t now = millis();

        for (uint8_t i = 0; i < activeSparkleCount; ++i)
        {
          if (!sparkleActive[i]
              && static_cast<int32_t>(now - nextSparkleAt[i]) >= 0)
          {
            chooseSparkle(i);
            sparkleActive[i] = true;
            sparkleStartedAt[i] = now;
          }

          if (sparkleActive[i])
          {
            const uint32_t elapsed = now - sparkleStartedAt[i];
            const uint8_t intensity = sparkleIntensity(elapsed);

            if (elapsed >= SPARKLE_FLASH_MS)
            {
              sparkleActive[i] = false;
              nextSparkleAt[i] = now + randomizedSparkleIntervalMs();
            }
            else if (intensity > 0)
            {
              if (sparkleLed[i] < NUM_LEDS)
              {
                const CRGB sparkleColor = CHSV(sparkleHue[i], 255, intensity);
                CRGBW &pixel = leds[sparkleLed[i]];
                pixel.r = qadd8(pixel.r, sparkleColor.r);
                pixel.g = qadd8(pixel.g, sparkleColor.g);
                pixel.b = qadd8(pixel.b, sparkleColor.b);
              }
            }
          }
        }

        brightnessPhaseA += brightnessTimeStepA;
        brightnessPhaseB += brightnessTimeStepB;
        colorPhaseA += colorTimeStepA;
        colorPhaseB += colorTimeStepB;
      }
    }
};


lightShows.push_back(new GlowSparkle02LightShow);
