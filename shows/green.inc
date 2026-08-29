#include "flummylights.h"




class GreenLightShow:public LightShow
{
  private:

 
  public:
  // constructor
  GreenLightShow()
  {
    
  
  }

  // get the show name
  const char *  getShowName()
  {
    return "green";
  }
  // get the parameter section key
  const char * getShowKey()
  {
    return "green";
  }
  // get background color for web page
  virtual const char * getPageColor()
  {
    return "#228B22";
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
    EVERY_N_MILLISECONDS(500) 
    {                           
      static uint8_t blink=255; 
      for (int i=0;i<NUM_LEDS;i++)
        leds[i] = CRGB(0,blink,0);
      blink=255-blink;
    }

  } 
  

};


lightShows.push_back(new(GreenLightShow));

