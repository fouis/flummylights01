#include "flummylights.h"
#include "generator.h"

class Laufflaeche02LightShow : public LightShow
{
  private:
    enum : uint16_t { FRAME_MS = 20 };

    struct Direction
    {
      float x;
      float y;
      float z;
    };

    float planeFrequency;
    float hueFrequency;
    float ledDistance;
    float directionSpeed;

    uint16_t planePhase;
    uint16_t planeStep;
    uint16_t huePhase;
    uint16_t hueStep;

    Direction directionFrom;
    Direction directionTo;
    Direction directionCurrent;
    float directionProgress;
    uint8_t targetCounter;
    uint8_t axisTarget;

    static Direction normalized(float x, float y, float z)
    {
      const float length = sqrtf(x * x + y * y + z * z);
      if (length <= 0.0001f)
      {
        return {1.0f, 0.0f, 0.0f};
      }
      return {x / length, y / length, z / length};
    }

    static Direction interpolateDirection(const Direction &a, const Direction &b, float t)
    {
      // Smoothstep keeps the changes in plane orientation calm at the
      // beginning and end of every transition.
      const float smoothT = t * t * (3.0f - 2.0f * t);
      return normalized(
        a.x + (b.x - a.x) * smoothT,
        a.y + (b.y - a.y) * smoothT,
        a.z + (b.z - a.z) * smoothT
      );
    }

    Direction nextDirectionTarget()
    {
      ++targetCounter;

      // Every fourth target is an exact coordinate axis. This guarantees
      // that the clearly recognizable X/Y/Z orientations occur regularly,
      // while the intermediate targets keep the motion spatially varied.
      if ((targetCounter % 4) == 0)
      {
        Direction result;
        if (axisTarget == 0)
        {
          result = {1.0f, 0.0f, 0.0f};
        }
        else if (axisTarget == 1)
        {
          result = {0.0f, 1.0f, 0.0f};
        }
        else
        {
          result = {0.0f, 0.0f, 1.0f};
        }
        axisTarget = (axisTarget + 1) % 3;
        return result;
      }

      // Constrained randomness: all components remain positive and none of
      // the random targets becomes almost axis-parallel by accident.
      const float x = static_cast<float>(random(20, 101)) / 100.0f;
      const float y = static_cast<float>(random(20, 101)) / 100.0f;
      const float z = static_cast<float>(random(20, 101)) / 100.0f;
      return normalized(x, y, z);
    }

    void advanceDirection()
    {
      directionProgress += directionSpeed * (static_cast<float>(FRAME_MS) / 1000.0f);

      while (directionProgress >= 1.0f)
      {
        directionProgress -= 1.0f;
        directionFrom = directionTo;
        directionTo = nextDirectionTarget();
      }

      directionCurrent = interpolateDirection(directionFrom, directionTo, directionProgress);
    }

    float maximumProjection() const
    {
      return
        directionCurrent.x * static_cast<float>(LED_SPACE_SIZE_X - 1) +
        directionCurrent.y * static_cast<float>(LED_SPACE_SIZE_Y - 1) +
        directionCurrent.z * static_cast<float>(LED_SPACE_SIZE_Z - 1);
    }

    float planePosition() const
    {
      // One cycle moves the plane from one side of the projected LED space
      // to the other and back again.
      const float normalizedPosition =
        (32767.0f - static_cast<float>(cos16(planePhase))) / 65534.0f;
      return normalizedPosition * maximumProjection();
    }

    float projectionForLed(uint8_t ledIndex) const
    {
      const LedPosition &p = ledPositions[ledIndex];
      return
        directionCurrent.x * static_cast<float>(p.x) +
        directionCurrent.y * static_cast<float>(p.y) +
        directionCurrent.z * static_cast<float>(p.z);
    }

  public:
    Laufflaeche02LightShow()
      : planeFrequency(1.0f),
        hueFrequency(0.25f),
        ledDistance(0.5f),
        directionSpeed(0.08f),
        planePhase(0),
        planeStep(0),
        huePhase(0),
        hueStep(0),
        directionFrom{1.0f, 0.0f, 0.0f},
        directionTo{0.5773503f, 0.5773503f, 0.5773503f},
        directionCurrent{1.0f, 0.0f, 0.0f},
        directionProgress(0.0f),
        targetCounter(0),
        axisTarget(1)
    {
      addPar(new FloatParameter("FREQ", &planeFrequency, 1.0f));
      addPar(new FloatParameter("DIRSPD", &directionSpeed, 0.08f));
      addPar(new FloatParameter("HUEFREQ", &hueFrequency, 0.25f));
      addPar(new FloatParameter("DIST", &ledDistance, 0.5f));
    }

    const char * getShowName()
    {
      return "Laufflaeche 02";
    }

    const char * getShowKey()
    {
      return "laufflaeche02";
    }

    const char * getPageColor()
    {
      return "#29374f";
    }

    void initShow()
    {
      planePhase = 0;
      huePhase = 0;
      directionFrom = {1.0f, 0.0f, 0.0f};
      directionTo = normalized(1.0f, 1.0f, 1.0f);
      directionCurrent = directionFrom;
      directionProgress = 0.0f;
      targetCounter = 0;
      axisTarget = 1;
      parChanged();
    }

    void parChanged()
    {
      planeFrequency = constrain(planeFrequency, 0.5f, 5.0f);
      directionSpeed = constrain(directionSpeed, 0.02f, 0.5f);
      hueFrequency = constrain(hueFrequency, 0.1f, 2.0f);
      ledDistance = constrain(ledDistance, 0.1f, 1.0f);

      planeStep = Generator::phaseStepFromHz(planeFrequency, FRAME_MS);
      hueStep = Generator::phaseStepFromHz(hueFrequency, FRAME_MS);
    }

    void lightLoop()
    {
      EVERY_N_MILLISECONDS(FRAME_MS)
      {
        advanceDirection();

        const float currentPlanePosition = planePosition();
        const uint8_t hue = static_cast<uint8_t>(huePhase >> 8);
        const CRGB color = CHSV(hue, 255, 255);

        // directionCurrent is normalized, therefore the difference between
        // the projected LED coordinate and plane position is the Euclidean
        // point-to-plane distance.
        for (uint8_t i = 0; i < NUM_LEDS; ++i)
        {
          const float distance = fabsf(projectionForLed(i) - currentPlanePosition);
          if (distance <= ledDistance)
          {
            leds[i] = color;
          }
          else
          {
            leds[i] = CRGBW(0, 0, 0, 0);
          }
        }

        planePhase += planeStep;
        huePhase += hueStep;
      }
    }
};

lightShows.push_back(new Laufflaeche02LightShow);
