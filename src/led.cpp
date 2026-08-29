
#include "flummylights.h"
#include "rgbw_output.h"



void initLED(void)
{
    rgbwOutputInit(DATA_PIN, NUM_LEDS);
}


// LED loop
void LEDTaskCode( void * pvParameters )
{
  Serial.print("LEDTask running on core ");
  Serial.println(xPortGetCoreID());

  initLED();


  for(;;)
  {
    bool showChanged = false;

    if (xSemaphoreTake(lightShowMutex, portMAX_DELAY) == pdTRUE)
    {
      int active = activelightShow.load();

      if (isValidShowIndex(active))
      {
        //Serial.println("doing light show");
        lightShows[active]->lightLoop();
      }

      int requested = newLightShow.load();
      if (requested != active && isValidShowIndex(requested))
      {
        lightShows[requested]->initShow();
        activelightShow.store(requested);
        showChanged = true;
      }

      xSemaphoreGive(lightShowMutex);
    }

    if (showChanged)
    {
      writePersistent();
    }

    // Hardware output is centralized here. Shows only update leds[].
    // 20 ms = 50 FPS and is known to be stable with the RGBW RMT driver.
    EVERY_N_MILLISECONDS(20)
    {
      rgbwOutputShow(leds, NUM_LEDS);
    }

    vTaskDelay(1);
  }
}

