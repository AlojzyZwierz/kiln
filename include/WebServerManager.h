#pragma once
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <Arduino.h>
#include "CurveManager.h"
#include "SystemState.h"
#include "ProcessController.h"
#include "MeasurementManager.h"
#include "StorageManager.h"
#include <TFT_eSPI.h>

class WebServerManager
{
private:
    AsyncWebServer server;

    const char *apSSID     = "kiln";
    const char *apPassword = "abcd1234";

    static constexpr unsigned long RECONNECT_INTERVAL = 30000UL; // ms

    CurveManager      &curveManager;
    TemperatureSensor &temperatureSensor;
    TFT_eSPI          &tft;

    bool          _staConnected      = false;
    bool          _apActive          = false;
    unsigned long _lastReconnectAt   = 0;

    void startAP();
    void tryConnect();          // nieblokujące WiFi.begin
    void registerWiFiEvents();
    void setupRoutes();

    String generateSVG(const Curve &curIn);

public:
    WiFiCredentials wifiCreds;

    WebServerManager(CurveManager &cm, TemperatureSensor &ts, TFT_eSPI &display)
        : server(80), curveManager(cm), temperatureSensor(ts), tft(display) {}

    void begin();
    void handleClient();        // wywołuj z loop() — lekkie
};