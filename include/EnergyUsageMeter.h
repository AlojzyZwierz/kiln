#pragma once
#include <Arduino.h>
class EnergyUsageMeter
{
public:
    void startMeasurement();
    void stopMeasurement();

    float getEnergyKWh() const;
    float getCost() const;

    String getCostString() const
    {
        float cost = calculateCost();
        return (cost == 0.0f) ? "" : String(getCost(), 4) + " zł";
    }

    void reset();

private:
    unsigned long heatingStartTime = 0;
    unsigned long totalHeatingTimeMs = 0;

    float calculateEnergyKWh() const;
    float calculateCost() const;
    bool isCounting =false;
};
