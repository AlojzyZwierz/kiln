#pragma once

#include <Adafruit_MAX31856.h>

class TemperatureSensor {
public:
    
TemperatureSensor(); //
    void begin();
    void update(); // próbuje odczytać nową temperaturę
    float getTemperature() const;
    int getErrorCount() const;
    bool hasError() const;

private:
    
    Adafruit_MAX31856 thermocouple;

    float lastValidTemperature = 0.0f;
    int errorCount = 0;

    static constexpr int MAX_ERRORS = 50;

    void handleError();


};