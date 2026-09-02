#include "flummylights.h"
#include "generator.h"

class Lauflicht01LightShow : public LightShow
{
  private:
    enum : uint16_t { FRAME_MS = 20 };

    float speedLedsPerSecond;
    float hueFrequency;
    int runLength;

    float position;
    int8_t direction;
    uint16_t huePhase;
    uint16_t hueStep;

  public:
    Lauflicht01LightShow()
      : speedLedsPerSecond(3.0f),
        hueFrequency(0.10f),
        runLength(1),
        position(0.0f),
        direction(1),
        huePhase(0),
        hueStep(0)
    {
      addPar(new FloatParameter("SPEED", &speedLedsPerSecond, 3.0f));
      addPar(new FloatParameter("HUEFREQ", &hueFrequency, 0.10f));
      addPar(new IntParameter("LENGTH", &runLength, 1));
    }

    const char * getShowName()
    {
      return "Lauflicht 01";
    }

    const char * getShowKey()
    {
      return "lauflicht01";
    }

    const char * getPageColor()
    {
      return "#172554";
    }

    void initShow()
    {
      position = 0.0f;
      direction = 1;
      huePhase = 0;
      parChanged();
    }

    void parChanged()
    {
      speedLedsPerSecond = constrain(speedLedsPerSecond, 1.0f, 10.0f);
      hueFrequency = constrain(hueFrequency, 0.03f, 0.50f);
      runLength = constrain(runLength, 1, 5);

      const float lastPosition = static_cast<float>(NUM_LEDS - runLength);
      position = constrain(position, 0.0f, lastPosition);

      hueStep = Generator::phaseStepFromHz(
        hueFrequency,
        FRAME_MS
      );
    }

    void lightLoop()
    {
      EVERY_N_MILLISECONDS(FRAME_MS)
      {
        for (uint8_t i = 0; i < NUM_LEDS; ++i)
        {
          leds[i] = CRGBW(0, 0, 0, 0);
        }

        const uint8_t hue = static_cast<uint8_t>(huePhase >> 8);
        const CRGB baseColor = CHSV(hue, 255, 255);

        const uint8_t leftLed = static_cast<uint8_t>(floorf(position));
        const uint8_t blendAmount = static_cast<uint8_t>(
          (position - static_cast<float>(leftLed)) * 255.0f
        );

        // The bar consists of runLength full LEDs. During movement the
        // trailing edge fades out while the new leading LED fades in.
        CRGB trailingColor = baseColor;
        trailingColor.nscale8_video(255 - blendAmount);
        leds[leftLed] = trailingColor;

        for (int offset = 1; offset < runLength; ++offset)
        {
          const uint8_t ledIndex = leftLed + offset;
          if (ledIndex < NUM_LEDS)
          {
            leds[ledIndex] = baseColor;
          }
        }

        const uint8_t leadingLed = leftLed + runLength;
        if (blendAmount > 0 && leadingLed < NUM_LEDS)
        {
          CRGB leadingColor = baseColor;
          leadingColor.nscale8_video(blendAmount);
          leds[leadingLed] = leadingColor;
        }

        huePhase += hueStep;

        const float step = speedLedsPerSecond * (static_cast<float>(FRAME_MS) / 1000.0f);
        position += static_cast<float>(direction) * step;

        const float lastPosition = static_cast<float>(NUM_LEDS - runLength);

        if (position >= lastPosition)
        {
          position = 2.0f * lastPosition - position;
          direction = -1;
        }
        else if (position <= 0.0f)
        {
          position = -position;
          direction = 1;
        }
      }
    }
};

lightShows.push_back(new Lauflicht01LightShow);
