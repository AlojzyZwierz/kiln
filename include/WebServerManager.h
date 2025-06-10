
#pragma once
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h> // <- KONIECZNE DLA ESP32

#include <Arduino.h>
#include "CurveManager.h"
// #include "TemperatureSensor.h"

// #include "FakeFurnace.h"
#include "SystemState.h"
#include "ProcessController.h"
#include "MeasurementManager.h"
#include "StorageManager.h"
#include <TFT_eSPI.h>

class WebServerManager
{

private:
    // WiFiServer server;
    AsyncWebServer server;
    // char* ssid = "T-Mobile_Swiatlowod_2822";
    // char* password = "52963065802928863554";

    const char *apSSID = "kiln";
    const char *apPassword = "abcd1234";

    String generateSVG(const Curve &curIn);
    CurveManager &curveManager;
    TemperatureSensor &temperatureSensor;
    TFT_eSPI &tft;

public:
    WebServerManager(CurveManager &cm, TemperatureSensor &ts, TFT_eSPI &display )
        : server(80), curveManager(cm), temperatureSensor(ts), tft(display)
    {
        // server.begin();
        // Serial.println("Web server started");
        // Serial.println("IP address: " + WiFi.localIP().toString());
    }
    void begin();
    void handleClient();
    void StartWebServer();
    WiFiCredentials wifiCreds;
};
