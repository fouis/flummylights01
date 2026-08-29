


#include "flummylights.h"

Preferences preferences;

// keys
const char * flummylightsKey="flummylights";

// all LightShows in a vector
std::vector<LightShow *> lightShows;

// stores tab state
TabState_t tabState=SELECT_ACTIVE;

// Stores LED state
String ledState;
// current light show
std::atomic<int> activelightShow{0};
// new light show chosen by website
std::atomic<int> newLightShow{0};
// number of lightshows
int numShows;

// multicore task handles
TaskHandle_t LEDTask;
SemaphoreHandle_t lightShowMutex = nullptr;

// RGBW LED frame buffer
CRGBW leds[NUM_LEDS];

// switch to clear wifi credentials on startup
bool clearWifi;
