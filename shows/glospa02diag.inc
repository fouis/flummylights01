#include "flummylights.h"
#include "generator.h"
#include <math.h>


class GlowSparkle02DiagLightShow : public LightShow
{
  private:
    const uint16_t FRAME_MS = 20;

    // Change only this value for the diagnosis:
    // 0 = static warm color
    // 1 = one time-only sine wave, identical on all LEDs
    // 2 = one sine wave with time + spatial phase
    // 3 = two overlaid waves, fixed warm hue
    // 4 = complete warm wobble from glospa02, without sparkles
    const uint8_t DIAG_MODE = 0;

    uint16_t phaseA = 0;
    uint16_t phaseB = 21845;

    const uint16_t timeStepA = Generator::phaseStepFromHz(0.14f, FRAME_MS);
    const uint16_t timeStepB = Generator::phaseStepFromHz(-0.14f * 0.67f, FRAME_MS);
    const uint16_t spaceStepA = Generator::phaseStepFromCycles(1.4f, NUM_LEDS);
    const uint16_t spaceStepB = Generator::phaseStepFromCycles(-1.4f * 1.37f, NUM_LEDS);
/*
    static uint8_t sineTo8(uint16_t phase)
    {
      return static_cast<uint8_t>((static_cast<int32_t>(sin16(phase)) + 32768) >> 8);
    }
    */

  static uint8_t sineTo8(uint16_t phase)
{
    

    const float angle =
        static_cast<float>(phase) *
        (TWO_PI / 65536.0f);

    const float s = sinf(angle);

    return static_cast<uint8_t>(
        127.5f + 127.5f * s
    );
}  

  public:
    const char * getShowName()
    {
      return "Glow Sparkle 02 DIAG";
    }

    const char * getShowKey()
    {
      return "glospa02diag";
    }

    const char * getPageColor()
    {
      return "#704020";
    }

    void initShow()
    {
      phaseA = 0;
      phaseB = 21845;
      for (uint8_t i = 0; i < NUM_LEDS; ++i)
     {
        uint8_t value = 40 + i * 6;

        leds[i].r = value;
        leds[i].g = value / 4;
        leds[i].b = 0;
        leds[i].w = 0;
      }

 
    }

    void parChanged()
    {
    }

    void lightLoop()
    {
      EVERY_N_MILLISECONDS(FRAME_MS)
      {
        for (int ledIndex = 0; ledIndex < NUM_LEDS; ++ledIndex)
        {
          if (DIAG_MODE == 0)
          {
            //leds[ledIndex] = CHSV(8, 255, 160);
            for (uint8_t i = 0; i < NUM_LEDS; ++i)
            {
                uint8_t value = 40 + i * 6;

                leds[i].r = value;
                leds[i].g = value / 4;
                leds[i].b = 0;
                leds[i].w = 0;
            }

          }
          else if (DIAG_MODE == 1)
          { /*
            const uint8_t wave = sineTo8(phaseA);
            const uint8_t value = static_cast<uint8_t>(40 + scale8(wave, 200));
            leds[ledIndex] = CHSV(8, 255, value);
            */
            const uint8_t wave = sineTo8(phaseA);
            const uint8_t value = static_cast<uint8_t>(40 + scale8(wave, 200));

            leds[ledIndex].r = value;
            leds[ledIndex].g = value / 4;
            leds[ledIndex].b = 0;
            leds[ledIndex].w = 0;
          }
          else if (DIAG_MODE == 2)
          {
            const uint16_t ledPhase = phaseA + static_cast<uint16_t>(ledIndex) * spaceStepA;
            const uint8_t wave = sineTo8(ledPhase);
            const uint8_t value = static_cast<uint8_t>(40 + scale8(wave, 200));
            //leds[ledIndex] = CHSV(8, 255, value);
            leds[ledIndex].r = value;
            leds[ledIndex].g = value / 4;
            leds[ledIndex].b = 0;
            leds[ledIndex].w = 0;
          }
          else if (DIAG_MODE == 3)
          {
            const uint16_t ledPhaseA = phaseA + static_cast<uint16_t>(ledIndex) * spaceStepA;
            const uint16_t ledPhaseB = phaseB + static_cast<uint16_t>(ledIndex) * spaceStepB;
            const uint8_t waveA = sineTo8(ledPhaseA);
            const uint8_t waveB = sineTo8(ledPhaseB);
            const uint8_t wobble = static_cast<uint8_t>(
              (3U * static_cast<uint16_t>(waveA) + 2U * static_cast<uint16_t>(waveB)) / 5U
            );
            const uint8_t value = static_cast<uint8_t>(40 + scale8(wobble, 200));
            leds[ledIndex] = CHSV(8, 255, value);
          }
          else
          {
            const uint16_t ledPhaseA = phaseA + static_cast<uint16_t>(ledIndex) * spaceStepA;
            const uint16_t ledPhaseB = phaseB + static_cast<uint16_t>(ledIndex) * spaceStepB;
            const uint8_t waveA = sineTo8(ledPhaseA);
            const uint8_t waveB = sineTo8(ledPhaseB);
            const uint8_t wobble = static_cast<uint8_t>(
              (3U * static_cast<uint16_t>(waveA) + 2U * static_cast<uint16_t>(waveB)) / 5U
            );

            const uint8_t hue = static_cast<uint8_t>(2 + scale8(wobble, 28));
            const uint8_t value = static_cast<uint8_t>(95 + scale8(wobble, 125));
            leds[ledIndex] = CHSV(hue, 255, value);
          }
        }

        if (DIAG_MODE >= 1)
        {
          phaseA += timeStepA;
        }
        if (DIAG_MODE >= 3)
        {
          phaseB += timeStepB;
        }

      }
    }
};


lightShows.push_back(new GlowSparkle02DiagLightShow);
