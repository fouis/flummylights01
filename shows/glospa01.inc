#include "flummylights.h"
#include "generator.h"

#define BASETIMESTEP 50
// steps per led in chain
#define RUNNERSTEPS 2048



class GlowSparkle01LightShow:public LightShow
{
  private:

    SineGenerator glowValGen;
    SineGenerator glowShiftGen;
    uint8_t valBias;
    uint8_t satBias;
    uint16_t sparkleRunner[4];
    int16_t sparkleStep[4];

  public:
  // constructor
  GlowSparkle01LightShow()
  {
    glowValGen.init(BASETIMESTEP,0.12,50);
    glowShiftGen.init(BASETIMESTEP,0.045,5);
    valBias=180;
    satBias=255;
    sparkleStep[0]=1.4*BASETIMESTEP;
    sparkleStep[1]=2.6*BASETIMESTEP;
    sparkleStep[2]=-0.45*BASETIMESTEP;
    sparkleStep[3]=-1.1*BASETIMESTEP;
    initShow();
  }


  // initialize the light show before running
  void initShow(void)
  {
      glowValGen.init();
      glowShiftGen.init();
      sparkleRunner[0]=4.5*RUNNERSTEPS;
      sparkleRunner[1]=11.7*RUNNERSTEPS;
      sparkleRunner[2]=19.5*RUNNERSTEPS;
      sparkleRunner[3]=27.3*RUNNERSTEPS;
      // init additional parameters
      parChanged();
  }

  // initialize after parameter change
  void parChanged(void)
  {
  }



  // get the show name
  const char *  getShowName()
  {
    return "Glow Sparkle Show";
    
  }
  // get the parameter section key
  const char * getShowKey()
  {
    return "glospa01";
  }

  // get background color for web page
  virtual const char * getPageColor()
  {
    return "#888B88";
  }

  
  
  
  // run the light show (in a loop)
  void lightLoop(void)
  {
    int32_t glowVal;
    int32_t shiftPhase;
    // FastLED based non-blocking delay to update/display the sequence.
    EVERY_N_MILLISECONDS(BASETIMESTEP) 
    {
      // get global pars
      glowVal=glowValGen.step();
      shiftPhase=glowShiftGen.step();

      // do it for every LED
      for (int l=0;l<NUM_LEDS;l++)
      {
        uint16_t phase;
        int8_t glowDiff=(int8_t)(glowVal>>16);
        phase=(shiftPhase+l*5000)&0xFFFF;
        int8_t shiftDiff=(int8_t)(((int)20*(sin16(phase)))>>16);
        uint8_t val=valBias+(uint8_t)(glowDiff+shiftDiff); 
        uint8_t sat=satBias;
        uint8_t hue=0;
        for (int i=0;i<4;i++)
        {
          int16_t sparkleDist=l*RUNNERSTEPS-(int)sparkleRunner[i];
          if (abs(sparkleDist)<RUNNERSTEPS)
          {
            hue+=(RUNNERSTEPS-abs(sparkleDist))/(RUNNERSTEPS/30);
          }
        }
        if (hue>30)hue=30;
        leds[l] = CHSV(hue,sat,val);
      }
      // stransfer to LEDs
      for(int i=0;i<4;i++)
      {
        sparkleRunner[i]+=sparkleStep[i];
        // overflow at 65536
        /*
        if (sparkleRunner[i]>16384)
        {
          sparkleRunner [i]=0;
        }
        */
      } 
    }
  }
};


lightShows.push_back(new(GlowSparkle01LightShow));








