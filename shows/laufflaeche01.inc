#include "flummylights.h"
#include "generator.h"


class Laufflaeche01LightShow : public LightShow
{
  private:
    enum : uint16_t { FRAME_MS = 20 };

    int repetitions;
    float planeFrequency;
    float hueFrequency;
    float ledDistance;

    uint8_t activeAxis;
    uint8_t completedCycles;
    uint16_t planePhase;
    uint16_t planeStep;
    uint16_t huePhase;
    uint16_t hueStep;

    float axisMaximum() const
    {
      if (activeAxis == 0)
      {
        return static_cast<float>(LED_SPACE_SIZE_X - 1);
      }
      if (activeAxis == 1)
      {
        return static_cast<float>(LED_SPACE_SIZE_Y - 1);
      }
      return static_cast<float>(LED_SPACE_SIZE_Z - 1);
    }

    float coordinateForLed(uint8_t ledIndex) const
    {
      const LedPosition &p = ledPositions[ledIndex];
      if (activeAxis == 0)
      {
        return static_cast<float>(p.x);
      }
      if (activeAxis == 1)
      {
        return static_cast<float>(p.y);
      }
      return static_cast<float>(p.z);
    }

    float planePosition() const
    {
      // One cycle moves the plane smoothly 0 -> maximum -> 0.
      const float normalized =
        (32767.0f - static_cast<float>(cos16(planePhase))) / 65534.0f;
      return normalized * axisMaximum();
    }

    void advancePlane()
    {
      const uint16_t oldPhase = planePhase;
      planePhase += planeStep;

      if (planePhase < oldPhase)
      {
        ++completedCycles;
        if (completedCycles >= repetitions)
        {
          completedCycles = 0;
          activeAxis = (activeAxis + 1) % 3;
          planePhase = 0;
        }
      }
    }

  public:
    Laufflaeche01LightShow()
      : repetitions(3),
        planeFrequency(1.0f),
        hueFrequency(0.25f),
        ledDistance(1.0f),
        activeAxis(0),
        completedCycles(0),
        planePhase(0),
        planeStep(0),
        huePhase(0),
        hueStep(0)
    {
      addPar(new IntParameter("N", &repetitions, 3));
      addPar(new FloatParameter("FREQ", &planeFrequency, 1.0f));
      addPar(new FloatParameter("HUEFREQ", &hueFrequency, 0.25f));
      addPar(new FloatParameter("DIST", &ledDistance, 1.0f));
    }

    const char * getShowName()
    {
      return "Laufflaeche 01";
    }

    const char * getShowKey()
    {
      return "laufflaeche01";
    }

    const char * getPageColor()
    {
      return "#253047";
    }

    void initShow()
    {
      activeAxis = 0;
      completedCycles = 0;
      planePhase = 0;
      huePhase = 0;
      parChanged();
    }

    void parChanged()
    {
      repetitions = constrain(repetitions, 1, 10);
      planeFrequency = constrain(planeFrequency, 0.5f, 5.0f);
      hueFrequency = constrain(hueFrequency, 0.1f, 2.0f);
      ledDistance = constrain(ledDistance, 0.5f, 4.0f);

      planeStep = Generator::phaseStepFromHz(planeFrequency, FRAME_MS);
      hueStep = Generator::phaseStepFromHz(hueFrequency, FRAME_MS);
    }

    void lightLoop()
    {
      EVERY_N_MILLISECONDS(FRAME_MS)
      {
        const float currentPlanePosition = planePosition();
        const uint8_t hue = static_cast<uint8_t>(huePhase >> 8);
        const CRGB color = CHSV(hue, 255, 255);

        for (uint8_t i = 0; i < NUM_LEDS; ++i)
        {
          const float distance = fabsf(coordinateForLed(i) - currentPlanePosition);
          if (distance <= ledDistance)
          {
            leds[i] = color;
          }
          else
          {
            leds[i] = CRGBW(0, 0, 0, 0);
          }
        }

        advancePlane();
        huePhase += hueStep;
      }
    }
};

lightShows.push_back(new Laufflaeche01LightShow);
