#include "EnergyUsageMeter.h"
#include "SettingsManager.h"

void EnergyUsageMeter::startMeasurement() {
    heatingStartTime = millis();
}

void EnergyUsageMeter::stopMeasurement() {
    unsigned long now = millis();
    if (heatingStartTime != 0) {
        totalHeatingTimeMs += (now - heatingStartTime);
        heatingStartTime = 0;
    }
}

float EnergyUsageMeter::getEnergyKWh() const {
    return calculateEnergyKWh();
}

float EnergyUsageMeter::getCost() const {
    return calculateCost();
}

void EnergyUsageMeter::reset() {
    totalHeatingTimeMs = 0;
    heatingStartTime = 0;
}

float EnergyUsageMeter::calculateEnergyKWh() const {
    float powerWatts = SettingsManager::get().getSettings().kilnPower; // np. 1500
    float hours = totalHeatingTimeMs / 3600000.0f;
    return powerWatts * hours ; // kWh
}

float EnergyUsageMeter::calculateCost() const {
    float pricePerKWh = SettingsManager::get().getSettings().unitCost; // np. 0.90 zł
    return getEnergyKWh() * pricePerKWh;
}
