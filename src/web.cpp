#include "Arduino.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
  
#include "SPIFFS.h"
#include <Preferences.h>
#include "flummylights.h"

#include "wifi_manager.h"



// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String getOptions()
{
  String output=String();
  for (int i=0;i<numShows;i++)
  {
    if (lightShows[i]!=NULL)
    {
      output+=String(F("<option value='"));
      output+=String(i);
      output+=String(F("'"));
      if (i == activelightShow.load())
      {
        output+=String(F(" selected "));
      }
      output+=String(F(">"));
      output+=String(lightShows[i]->getShowName());
      output+=String(F("</option>\r\n"));
    }
  }
  return output;
}

String getPresets()
{
  // todo: output real presets here
  String output=String();
  output+="<option value='1'>Hans</option>";
  output+="<option value='2'>Helga</option>";
  output+="<option value='3'>Hannelore</option>";
  return output;
}

String showFiles()
{
  String filelist="<html><head></head><body><table>";
  File root = SPIFFS.open("/");
 
  File file = root.openNextFile();

  size_t fs_size=0;
 
  while(file){

      filelist+="<tr><td>";
      filelist+=file.name();
      filelist+="</td><td>";
      filelist+=file.size();
      filelist+="</td></tr>";
      fs_size+=file.size();
 
      file = root.openNextFile();
  }
  filelist+="<tr><td></td></tr><tr><td>overall size:</td><td>";
  filelist+=fs_size;
  filelist+="</td></tr></table></body>";
  return filelist;
}


// Replaces placeholder with LED state value
String processor(const String& var)
{
  File includeFile;
  //Serial.println(var);
  /*
  if(var == "STATE")
  {
    if(digitalRead(LED_BUILTIN)){
      ledState = "ON";
    }
    else{
      ledState = "OFF";
    }
    Serial.print(ledState);
    return ledState;
  }
  */
  if (var=="SHOW")
  {
    int active = activelightShow.load();
    if (isValidShowIndex(active)) return String(lightShows[active]->getShowName());
    else return String();
  }
  else if (var=="SHOWBOX")
  {
    int active = activelightShow.load();
    if (isValidShowIndex(active))
    {
      String filename="/";
      filename+=String(lightShows[active]->getShowKey());
      filename+=".html";
      if (SPIFFS.exists (filename))
      {
        includeFile=SPIFFS.open(filename, "r");
        String page = includeFile.readString();
        includeFile.close();

        if (xSemaphoreTake(lightShowMutex, portMAX_DELAY) == pdTRUE)
        {
          String inc = lightShows[active]->filterWeb(page);
          xSemaphoreGive(lightShowMutex);
          return inc;
        }
        return String();
      }
      else
      {
        return "<h1>"+String(lightShows[active]->getShowName())+"</h1>";
      }
    }
    else return String();
  }
  else if (var=="SHOWOPTIONS")
  {
    return getOptions(); 
  }
  else if (var=="PRESETOPTIONS")
  {
    return getPresets(); 
  }

  else if (var=="PAGECOLOR")
  {
    int active = activelightShow.load();
    if (isValidShowIndex(active))
    {
      return lightShows[active]->getPageColor();
    }
    else
    {
      return String();
    }
  }
  else if (var=="HIDDEN")
  {
    return "hidden";
  }
  else if (var=="STATIC")
  {
    return "no";
  }
  else if (var=="SELECT_ACTIVE")
  {
    if (tabState==SELECT_ACTIVE) return "show active";
  }
  else if (var=="PARAMETER_ACTIVE")
  {
    if (tabState==PARAMETER_ACTIVE) return "show active";
    
  }
  else if (var=="INFO_ACTIVE")
  {
    if (tabState==INFO_ACTIVE) return "show active";
    
  }
  return String();
}

void initWeb(void)
{



  initWifi(clearWifi);
  


  

  

  

  
  

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    tabState=SELECT_ACTIVE;
    //Check if GET parameter exists
    if(request->hasParam("show"))
    {
      const AsyncWebParameter* showpar = request->getParam("show");
      int requestedShow = showpar->value().toInt();
      if (isValidShowIndex(requestedShow))
      {
        newLightShow.store(requestedShow);
      }
      //writePersistent();
    }
    Serial.print("free memory:");
    Serial.println(ESP.getFreeHeap());
    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });


  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request){
    tabState=SELECT_ACTIVE;
    //Check if POST parameter exists
    if(request->hasParam("show",true))
    {
      const AsyncWebParameter* showpar = request->getParam("show",true);
      int requestedShow = showpar->value().toInt();
      if (isValidShowIndex(requestedShow))
      {
        newLightShow.store(requestedShow);
      }
      //writePersistent();
    }
    else 
    {
      if (xSemaphoreTake(lightShowMutex, portMAX_DELAY) == pdTRUE)
      {
        int active = activelightShow.load();
        if (isValidShowIndex(active) && lightShows[active]->processForm(request))
        {
          preferences.begin(lightShows[active]->getShowKey(),false);
          lightShows[active]->writePersistent(&preferences);
          preferences.end();
          tabState=PARAMETER_ACTIVE;
          lightShows[active]->parChanged();
        }
        xSemaphoreGive(lightShowMutex);
      }
    }
    Serial.print("free memory:");
    Serial.println(ESP.getFreeHeap());
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Lightweight endpoint for immediate parameter updates.
  // POST fields: key=<parameter>, value=<value>, save=0|1
  server.on("/api/parameter", HTTP_POST, [](AsyncWebServerRequest *request){
    if (!request->hasParam("key", true) || !request->hasParam("value", true))
    {
      request->send(400, "text/plain", "Missing key or value");
      return;
    }

    const String key = request->getParam("key", true)->value();
    const String value = request->getParam("value", true)->value();
    const bool save = request->hasParam("save", true) && request->getParam("save", true)->value() == "1";
    bool changed = false;

    if (xSemaphoreTake(lightShowMutex, portMAX_DELAY) == pdTRUE)
    {
      const int active = activelightShow.load();
      if (isValidShowIndex(active))
      {
        if (save)
        {
          preferences.begin(lightShows[active]->getShowKey(), false);
          changed = lightShows[active]->setParameter(key, value, &preferences);
          preferences.end();
        }
        else
        {
          changed = lightShows[active]->setParameter(key, value);
        }

        if (changed)
        {
          lightShows[active]->parChanged();
        }
      }
      xSemaphoreGive(lightShowMutex);
    }

    if (changed) request->send(204);
    else request->send(404, "text/plain", "Unknown parameter");
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to javascript file
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/script.js", "text/js");
  });

  
  server.on("/reset",HTTP_GET,[](AsyncWebServerRequest *request){
    preferences.begin(flummylightsKey,false);
    preferences.putBool(clearWifiKey,true);
    preferences.end();
    request->send(200, "text/plain", "Wifi credentials will be deleted on next start up");
  });

  server.on("/spiffs",HTTP_GET,[](AsyncWebServerRequest *request){
    request->send(200,"text/html",showFiles());
  });
  

  
  // Start server
  server.begin();

}
