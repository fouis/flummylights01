
#include "flummylights.h"





class BlueLightShow:public LightShow
{
  private:
    int blinkTime;
 
  public:
  // constructor
  BlueLightShow()
  {
    addPar (new IntParameter("BLINKTIME",&blinkTime,500)); 
  }

  // get the show name
  const char *  getShowName()
  {
    return "blue";
  }
  // get the parameter section key
  const char * getShowKey()
  {
    return "blue";
  }

  // get background color for web page
  virtual const char * getPageColor()
  {
    return "#0000ff";
  }


  
  // initialize the light show before running
  void initShow(void)
  {
    
  }

    // initialize after parameter change
  void parChanged(void)
  {

  }

  // run the light show (in a loop)
  void lightLoop(void)
  {
    // FastLED based non-blocking delay to update/display the sequence.
    EVERY_N_MILLIS_I(thisTimer,blinkTime) 
    {   
      thisTimer.setPeriod(blinkTime);                       
      static uint8_t blink=255; 
      for (int i=0;i<NUM_LEDS;i++)
        leds[i] = CRGB(0,0,blink);
      blink=255-blink;
    }

  }  

  
};

lightShows.push_back(new(BlueLightShow));