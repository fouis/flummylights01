#include "flummylights.h"





class RedLightShow:public LightShow
{
  private:
    bool bikechecked;
    bool dodgechecked;
    bool booster;
    bool overdrive;
    int16_t colorindex;
    int16_t instrument;
    float weight;
  public:
  // constructor
  RedLightShow()
  {
    addPar(new BoolCheckedParameter("BIKE",&bikechecked,false)); 
    addPar(new BoolCheckedParameter("DODGE",&dodgechecked,false));
    addPar(new BoolCheckedParameter("BOOSTER",&booster,false));
    addPar(new BoolCheckedParameter("OVERDRIVE",&overdrive,true));
    addPar(new RadioCheckedParameter("COLOR",&colorindex,0)); 
    addPar(new SelectParameter("INSTRUMENT",&instrument,0)); 
    addPar(new FloatParameter("WEIGHT",&weight,5.0));
    
  }


  // get the show name
  const char *  getShowName()
  {
    return "red";
  }
  // get the parameter section key
  const char * getShowKey()
  {
    return "red";
  }

  // get background color for web page
  virtual const char * getPageColor()
  {
    return "#ff0000";
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
    static uint8_t brightness=255;
    // FastLED based non-blocking delay to update/display the sequence.
    EVERY_N_MILLISECONDS(10) 
    {                           
      static uint8_t hue = 0; 
      if (bikechecked)
      {
        brightness++;
      }
      leds[0] = CHSV(hue++, 255, brightness);
      leds[1]= CHSV(255-hue, 255, brightness);
    }


  } 
  
 
};

lightShows.push_back(new(RedLightShow));

