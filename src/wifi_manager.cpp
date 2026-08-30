#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>

#include "wifi_manager.h"

static bool wifiConfigChanged = false;

static void saveConfigCallback()
{
    wifiConfigChanged = true;
}

void initWifi(bool clearWifi)
{
    wifiConfigChanged = false;
    WiFi.setHostname("flummy");
    WiFiManager wifiManager;

    wifiManager.setAPStaticIPConfig(
        IPAddress(192, 168, 0, 1),
        IPAddress(192, 168, 0, 1),
        IPAddress(255, 255, 255, 0)
    );

    wifiManager.setSaveConfigCallback(saveConfigCallback);

    if (clearWifi)
        wifiManager.resetSettings();

    bool connected = wifiManager.autoConnect("Flummy");

    if (wifiConfigChanged)
    {
        Serial.println("WiFi configuration changed - restarting");
        delay(500);
        ESP.restart();
    }

    if (connected && WiFi.status() == WL_CONNECTED)
    {
        Serial.print("Connected. Local IP: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("WiFi connection failed");
    }
}