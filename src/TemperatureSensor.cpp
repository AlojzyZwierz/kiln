#include "TemperatureSensor.h"

TemperatureSensor::TemperatureSensor()
    : thermocouple(5, 23, 19, 18) // CS, DI, DO, CLK – zmień piny wg potrzeb
{
}
float TemperatureSensor::getCJTemperature()
{
    return thermocouple.readCJTemperature();
}
void TemperatureSensor::begin()
{
    thermocouple.begin();
    thermocouple.setThermocoupleType(MAX31856_TCTYPE_S);
}

void TemperatureSensor::update()
{
    float temp = thermocouple.readThermocoupleTemperature();
    lastErrorCode = thermocouple.readFault();

    if (lastErrorCode == 0 && !isnan(temp))
    {
        lastValidTemperature = temp;
        errorCount = 0;
    }
    else
    {
        if (SystemState::get().getMode() == SystemMode::Firing)
            handleError();
            SoundManager::playTone(300, 100); // Dźwięk błędu
            Serial.print("Temperature read error: " + String(lastErrorCode) + " Temp: " + String(temp));
    }
}

void TemperatureSensor::handleError()
{
    errorCount++;
    if (errorCount >= MAX_ERRORS)
    {
        // Tu można dodać np. flagę krytycznego błędu
    }
}

float TemperatureSensor::getTemperature() const
{
    return lastValidTemperature;
}

int TemperatureSensor::getErrorCount() const
{
    return errorCount;
}

bool TemperatureSensor::hasError() const
{
    return errorCount > 0;
}
