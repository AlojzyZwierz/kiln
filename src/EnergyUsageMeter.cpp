#include "EnergyUsageMeter.h"
#include "SettingsManager.h"

void EnergyUsageMeter::startMeasurement()
{
    heatingStartTime = millis();
    isCounting = true;
    // Serial.println("Starting energy measurement at: " + String(heatingStartTime));
}

void EnergyUsageMeter::stopMeasurement()
{
    
    unsigned long now = millis();
    if (heatingStartTime != 0)
    {
        totalHeatingTimeMs += (now - heatingStartTime);
        heatingStartTime = 0;
        isCounting = false;
    }
    // Serial.println("Stopping energy measurement at: " + String(now) + ", total heating time: " + String(totalHeatingTimeMs) + " ms");
}

float EnergyUsageMeter::getEnergyKWh() const
{
    return calculateEnergyKWh();
}

float EnergyUsageMeter::getCost() const
{
    // Serial.println("Calculating cost based on energy usage." + String(calculateCost(), 6) );
    return calculateCost();
}

void EnergyUsageMeter::reset()
{
    totalHeatingTimeMs = 0;
    heatingStartTime = 0;
}

float EnergyUsageMeter::calculateEnergyKWh() const
{
    float powerkWatts = SettingsManager::get().getSettings().kilnPower; // np. 1500
    float hours = (totalHeatingTimeMs +isCounting?(millis()-heatingStartTime):0 ) / 3600000.0f;
    return powerkWatts * hours; // kWh
    // Serial.println("Total heating time in ms: " + String(totalHeatingTimeMs));
}

float EnergyUsageMeter::calculateCost() const
{
    float pricePerKWh = SettingsManager::get().getSettings().unitCost; // np. 0.90 zł
    return getEnergyKWh() * pricePerKWh;
}
