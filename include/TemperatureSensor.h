#pragma once
#ifndef NO_THERMOCOUPLE
#include <Adafruit_MAX31856.h>
#endif
#include "SystemState.h"
#include "SoundManager.h"

class TemperatureSensor
{
public:
    TemperatureSensor(); //
    void begin();
    void update(); // próbuje odczytać nową temperaturę
    float getTemperature() const;
    int getErrorCount() const;
    bool hasError() const;
    uint8_t GetLastErrorCode() { return lastErrorCode; }; // zwraca kod ostatniego błędu
    float getCJTemperature();
    static constexpr int MAX_ERRORS = 30;
    float getRawVoltage();
    float getTemperatureFromRawVoltage(float rawVoltage);
    float readThermocoupleTemperature();
    float calcCorrectedTemp(float wrongTemp);

private:
#ifndef NO_THERMOCOUPLE
    Adafruit_MAX31856 thermocouple;
#endif
    float lastValidTemperature = 0.0f;
    int errorCount = 0;
    uint8_t lastErrorCode = 0; // kod ostatniego błędu, np. z MAX31856
    void handleError();
};