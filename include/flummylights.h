#ifndef FLUMMYLIGHTS_H
#define FLUMMYLIGHTS_H

#include "Arduino.h"

#include "ESPAsyncWebServer.h"

#include <Preferences.h>
#include <FastLED.h>
#include "FastLED_RGBW.h"
#include "parameter.h"
#include <vector>
#include <atomic>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>





/*
 * defines
 */
// How many leds in your strip?
#define NUM_LEDS 32

// For led chips like WS2812, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
// Clock pin only needed for SPI based chipsets when not using hardware SPI
#define DATA_PIN 27

/********************
 * enums
 */
enum TabState_t { SELECT_ACTIVE, PARAMETER_ACTIVE, INFO_ACTIVE };

/****************************
 * type definitions
 */


/**
 * class definitions
 */
// class for handling light show
class LightShow
{
  
  public:
    // get the show name
    virtual const char *  getShowName() = 0;
    // get the parameter section key
    virtual const char *  getShowKey() = 0;
    // get background color web page
    virtual const char * getPageColor() = 0;
    // write the persistent parameters
    virtual void writePersistent(Preferences * p);
    // read the persistent parameters
    virtual void readPersistent(Preferences * p);
    // process HTTP form data, returns true, if parameters changed
    virtual bool processForm(AsyncWebServerRequest *request);
    // initialize the light show before running
    virtual void initShow(void) = 0;
    // initialize after parameter change
    virtual void parChanged(void)=0;
    // run the light show (in a loop)
    virtual void lightLoop(void)  = 0;
    // filter the website part for the Show
    virtual String filterWeb(String);
    // register a parameter object
    void addPar(Parameter * p);
    // print all parameter keys
    void printDebugPar(void);

  protected:
    std::vector<Parameter *> allPars;
};

/*
 * globals
 */
extern Preferences preferences;


extern const char * flummylightsKey;
extern const char * clearWifiKey;


// stores tab state
extern TabState_t tabState;

// Stores LED state
extern String ledState;
// stored color state
extern std::atomic<int> activelightShow;
// new light show chosen by website
extern std::atomic<int> newLightShow;
// number of light shows
extern int numShows;

// switch to clear wifi credentials on startup
extern bool clearWifi;





// multicore task handles
extern TaskHandle_t LEDTask;

// protects LightShow state shared between the web and LED cores
extern SemaphoreHandle_t lightShowMutex;

// leds
extern CRGBW leds[NUM_LEDS];

class LightShow;
// lightshows
extern std::vector<LightShow *> lightShows;



/***
 * function definitions
 */
// helper functions
bool isValidShowIndex(int index);


// web functions
void initWeb();
// led functions
void initLED(void);
// LED loop
void LEDTaskCode( void * pvParameters );

void registerLightShows(void);


void writePersistent(void);
void readPersistent(void);



#endif
