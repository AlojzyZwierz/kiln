#include "TemperatureSensor.h"
#ifndef NO_THERMOCOUPLE
TemperatureSensor::TemperatureSensor()
    : thermocouple(MAX_CS, MAX_MOSI, MAX_MISO, MAX_CLK) // CS, DI, DO, CLK – zmień piny wg potrzeb
{
}
float TemperatureSensor::getCJTemperature()
{
    float cjTemp = thermocouple.readCJTemperature();
    //Serial.println("cjtemp: " + String(cjTemp));
    return cjTemp;
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
#else
TemperatureSensor::TemperatureSensor()
    // CS, DI, DO, CLK – zmień piny wg potrzeb
{
}
float TemperatureSensor::getCJTemperature()
{
    return 20.0f; // Zwracamy domyślną wartość, gdy nie używamy termopary
}
void TemperatureSensor::begin()
{
   
}

void TemperatureSensor::update()
{

}

void TemperatureSensor::handleError()
{

}

float TemperatureSensor::getTemperature() const
{
    return 30.0f; // Zwracamy domyślną wartość, gdy nie używamy termopary
}

int TemperatureSensor::getErrorCount() const
{
    return 0;
}

bool TemperatureSensor::hasError() const
{
    return false;
}
#endif
