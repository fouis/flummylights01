#include "flummylights.h"

class Blinky01LightShow : public LightShow
{
  private:
    int ledNumber;

  public:
    Blinky01LightShow()
      : ledNumber(0)
    {
      addPar(new IntParameter("LED", &ledNumber, 0));
    }

    const char * getShowName()
    {
      return "Blinky 01";
    }

    const char * getShowKey()
    {
      return "blinky01";
    }

    const char * getPageColor()
    {
      return "#343a40";
    }

    void initShow()
    {
      parChanged();
    }

    void parChanged()
    {
      ledNumber = constrain(ledNumber, 0, NUM_LEDS - 1);
    }

    void lightLoop()
    {
      EVERY_N_MILLISECONDS(250)
      {
        static bool blinkOn = false;
        blinkOn = !blinkOn;

        for (uint8_t i = 0; i < NUM_LEDS; ++i)
        {
          leds[i] = CRGBW(0, 0, 0, 0);
        }

        if (blinkOn)
        {
          // SK6812 RGBW: use the dedicated white channel at full intensity.
          leds[ledNumber] = CRGBW(0, 0, 0, 255);
        }
      }
    }
};

lightShows.push_back(new Blinky01LightShow);
