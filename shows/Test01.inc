#include "flummylights.h"


class Test01LightShow:public LightShow
{
  private:
    uint8_t numLeds;
    uint8_t sparkleHue;
    uint8_t sparkleSat;
    uint8_t sparkleVal;
    uint8_t waveHue;
    uint8_t waveSat;
    uint8_t waveVal;
    uint8_t valAmpl;
    float sparklePerc,wavePerc;
    uint16_t time1;
    uint16_t timestep1;
    int8_t green_index;
    int8_t green_dir;

  public:
  // constructor
  Test01LightShow()
  {
    numLeds=2; 
    timestep1=300;
    addPar(new Uint8Parameter("NUMLEDS",&numLeds,2));
    addPar(new Uint8Parameter("SPARKLEHUE",&sparkleHue,96));
    addPar(new Uint8Parameter("SPARKLESAT",&sparkleSat,96));
    addPar(new Uint8Parameter("SPARKLEVAL",&sparkleVal,200));
    addPar(new Uint8Parameter("WAVEHUE",&waveHue,96));
    addPar(new Uint8Parameter("WAVESAT",&waveSat,96));
    addPar(new Uint8Parameter("WAVEVAL",&waveVal,200));
    addPar (new FloatParameter("SPARKLEPERC",&sparklePerc,0.5));
    initShow();
  }


  // initialize the light show before running
  void initShow(void)
  {
      time1=0;
      green_index=0;
      green_dir=1;
      // init additional parameters
      parChanged();
  }

  // initialize after parameter change
  void parChanged(void)
  {
    valAmpl=80;
    if (waveVal<valAmpl) valAmpl=waveVal;
    else if (255-waveVal<valAmpl) valAmpl=255-waveVal; 
    wavePerc=1.0-sparklePerc;
  }



  // get the show name
  const char *  getShowName()
  {
    return "Test LED Show";
    
  }
  // get the parameter section key
  const char * getShowKey()
  {
    return "test01";
  }

  // get background color for web page
  virtual const char * getPageColor()
  {
    return "#888B88";
  }

  
  
  
  // run the light show (in a loop)
  void lightLoop(void)
  {
    // FastLED based non-blocking delay to update/display the sequence.
    EVERY_N_MILLISECONDS(200)
    {
        if (green_index==0)
        {
            green_dir=1;
        }
        else if (green_index>=numLeds)
        {
            green_dir=-1;
        }
        green_index+=green_dir;
    }
    EVERY_N_MILLISECONDS(10) 
    {   
      time1+=timestep1;
      uint8_t brightness=waveVal+(valAmpl*(sin16_C(time1)>>8)>>8);                          
      for (uint8_t ledi=0;ledi<numLeds;ledi++)
      {
        if (ledi==green_index)
        {
            leds[ledi]=CHSV(sparklePerc*sparkleHue+wavePerc*waveHue,sparklePerc*sparkleSat+wavePerc*waveSat,sparklePerc*sparkleVal+wavePerc*brightness);
        }
        else
        { 
            leds[ledi] = CHSV(waveHue, waveSat, brightness);
        }
      }
    }
  } 
};


lightShows.push_back(new(Test01LightShow));







