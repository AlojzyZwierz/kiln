#pragma once

#include <Adafruit_MAX31856.h>
#include "SystemState.h"

class TemperatureSensor {
public:
    
TemperatureSensor(); //
    void begin();
    void update(); // próbuje odczytać nową temperaturę
    float getTemperature() const;
    int getErrorCount() const;
    bool hasError() const;
    uint8_t GetLastErrorCode(){ return lastErrorCode; } ; // zwraca kod ostatniego błędu

private:
    
    Adafruit_MAX31856 thermocouple;

    float lastValidTemperature = 0.0f;
    int errorCount = 0;

    static constexpr int MAX_ERRORS = 50;
    uint8_t lastErrorCode = 0; // kod ostatniego błędu, np. z MAX31856
    void handleError();


};