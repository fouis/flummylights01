

// Import required libraries
#include "Arduino.h"
#include <WiFi.h>

#include "SPIFFS.h"
#include <Preferences.h>


#include "flummylights.h"

// key length max is 16 cahrs
const char * activeLightShowKey="activeshow";
const char * clearWifiKey="CLEARWIFI";

bool isValidShowIndex(int index)
{
  return index >= 0 && index < numShows && lightShows[index] != nullptr;
}

void readPersistent(void)
{
    preferences.begin(flummylightsKey,false);
    //preferences.clear();
    int storedLightShow = preferences.getInt(activeLightShowKey, 0);
    int active = isValidShowIndex(storedLightShow) ? storedLightShow : 0;
    activelightShow.store(active);
    newLightShow.store(active);
    clearWifi=preferences.getBool(clearWifiKey,false);
    if (clearWifi)
    {
      preferences.putBool(clearWifiKey, false);
    }
    preferences.end();
}

void writePersistent(void)
{
  preferences.begin(flummylightsKey,false);
  preferences.putInt(activeLightShowKey, activelightShow.load());
  preferences.end();  
}

void setup()
{
  int applicationCore;
  int ledCore;
  // Serial port for debugging purposes
  Serial.begin(115200);
  // register all lightShows
  registerLightShows();
  numShows=lightShows.size();
  
  
  pinMode(LED_BUILTIN, OUTPUT);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  Serial.println("reading persistent pars");
  Serial.print("number of light shows: ");
  Serial.println(numShows);
  // read persistent parameters
  // for the program
  readPersistent();
  // now for all shows
  for (int i=0;i<numShows;i++)
  {
    if (lightShows[i]!=NULL) 
    {
      preferences.begin(lightShows[i]->getShowKey(),false);
      //preferences.clear();
      lightShows[i]->readPersistent(&preferences);
      preferences.end();
      lightShows[i]->initShow();
    }
  }

  
  
  applicationCore = xPortGetCoreID();
  Serial.print("Application running on core ");
  Serial.println(xPortGetCoreID());

  lightShowMutex = xSemaphoreCreateMutex();
  if (lightShowMutex == nullptr)
  {
    Serial.println("Failed to create light show mutex");
    return;
  }

  Serial.println("creating led core");
  switch (applicationCore)
  {
    case 0: ledCore=1; break;
    case 1: ledCore=0; break;
    default: ledCore=-1;
  }
  if (ledCore>=0)
  {
    //create a task that will be executed in the LEDTaskCode() function, with priority 1 and executed on core 1
    xTaskCreatePinnedToCore(
                      LEDTaskCode,   /* Task function. */
                      "LEDTask",     /* name of task. */
                      10000,       /* Stack size of task */
                      NULL,        /* parameter of the task */
                      1,           /* priority of the task */
                      &LEDTask,      /* Task handle to keep track of created task */
                      ledCore);          /* pin task to core ledCore*/
      delay(500); 
  }


  initWeb();


}

 
void loop()
{
  static bool stateLed;
  EVERY_N_MILLISECONDS(500) 
  {
    // check if we are yet connected
    if (WiFi.status() == WL_CONNECTED) 
    {
      stateLed= !stateLed;
      digitalWrite(LED_BUILTIN, stateLed); 

    } 
  }
  
}

// class implementations

void LightShow::addPar(Parameter * p)
{
  allPars.push_back(p);
}

void LightShow::printDebugPar(void)
{
  for(std::size_t i = 0; i < allPars.size(); ++i) 
  {
    allPars[i]->printDebug();
  }
}

// write the persistent parameters
void LightShow::writePersistent(Preferences * p)
{
  // do it for all parameters
  for(std::size_t i = 0; i < allPars.size(); ++i) 
  {
    allPars[i]->writePers(p);
  }

}

// read the persistent parameters
void LightShow::readPersistent(Preferences * p)
{
  // do it for all parameters
  for(std::size_t i = 0; i < allPars.size(); ++i) 
  {
    allPars[i]->readPers(p);
  }
}

// filter the web output
String LightShow::filterWeb(String s)
{
  // do it for all parameters
  for(std::size_t i = 0; i < allPars.size(); ++i)
  {
    allPars[i]->filterWeb(&s);
  }
  return s; 
}

// process HTTP form data
bool LightShow::processForm(AsyncWebServerRequest *request)
{
  // print all parameters in request
  if(request->hasParam(getShowKey(),true))
  {
    // do it for all parameters
    for(std::size_t i = 0; i < allPars.size(); ++i)
    {
      allPars[i]->processForm(request);
    }
    return true;
  }
  return false;
}


