#include "flummylights.h"

// runs per computation



class ComptestLightShow:public LightShow
{
  private:
    // time for floating point operations
    float time_f;
    // time for integer operations
    uint16_t time_i;
    // time step for integer operations: 0.1° per step
    uint16_t timestep_i=18;
    // time step for floating point operations:0.1° per step
    float timestep_f=2*PI/3600.0;
    // time difference for floating point operation
    float difftime_f;
    // time difference for integer operation
    float difftime_i;



  public:
  // constructor
  ComptestLightShow()
  {

    initShow();
  }


  // initialize the light show before running
  void initShow(void)
  {
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
    return "Computation Test";
    
  }
  // get the parameter section key
  const char * getShowKey()
  {
    return "comptest";
  }

  // get background color for web page
  virtual const char * getPageColor()
  {
    return "#888B88";
  }

  
  // filter the web output
  String filterWeb(String s)
  {
    s=LightShow::filterWeb(s);
    String result="float time: ";
    result+=String(difftime_f);
    result+=" µs<br>";
    result+="int time: ";
    result+=String(difftime_i);
    result+=" µs<br>";

    s.replace(String("%RESULT%"),result);
    return s;
  }
  
  // run the light show (in a loop)
  void lightLoop(void)
  {
    int i;
    static uint8_t brightness=0;
    volatile static uint8_t val;
    uint8_t offset=128;
    uint64_t starttime,endtime;
    // do the integer test
    // get time in microseconds since start
    starttime=esp_timer_get_time();
    for (i=0;i<10000;i++)
    {
        time_i+=timestep_i;
        val=offset+(((uint32_t)sin16(time_i)*255)>>16);       
    }
    endtime=esp_timer_get_time();
    difftime_i=(endtime-starttime)/10000.0;
    /*
    Serial.print("Integer computation: ");
    Serial.print(difftime);
    Serial.println("µs");
    */
    // do the float test
    starttime=esp_timer_get_time();
    for (int i=0;i<1000;i++)
    {
        time_f+=timestep_f;
        if (time_f>2*PI)time_f-=2*PI;
        val=offset+(uint8_t)(127.0*sin(time_f));
    }
    endtime=esp_timer_get_time();
    difftime_f=(endtime-starttime)/1000.0;
    /*
    Serial.print("Float computation: ");
    Serial.print(difftime);
    Serial.println("µs");
    */

    // FastLED based non-blocking delay to update/display the sequence.
    EVERY_N_MILLISECONDS(500) 
    { 
      leds[0] = CHSV(0, 255, brightness);
      brightness=255-brightness;
    }
  } 
};


lightShows.push_back(new(ComptestLightShow));








