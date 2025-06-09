
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>  // <- KONIECZNE DLA ESP32

#include <Arduino.h>
#include "CurveManager.h"
//#include "TemperatureSensor.h"

//#include "FakeFurnace.h"
#include "SystemState.h"
#include "ProcessController.h"
#include "MeasurementManager.h"



class WebServerManager {

private:
    //WiFiServer server;
    AsyncWebServer server;
    char* ssid = "T-Mobile_Swiatlowod_2822";
    char* password = "52963065802928863554";

    const char* apSSID = "kiln";
    const char* apPassword = "11223344";

    String generateSVG(const Curve& curIn);
    CurveManager& curveManager;
    TemperatureSensor& temperatureSensor;

    public:
    WebServerManager(CurveManager& cm, TemperatureSensor& ts);
    void begin();
    void handleClient();
    void StartWebServer();

};
