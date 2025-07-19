#pragma once
#include <Arduino.h>
#include "EnergyUsageMeter.h"
#include "SystemState.h"

class HeatingController
{
public:
    HeatingController(EnergyUsageMeter &meter) : energyMeter(meter) {}
    void update();
    void setRatio(float r); // wartość PID: 0.0 - 1.0
    float getRatio();
    // void setEnabled(bool on);          // czy grzanie ma działać
    void setCycleTime(unsigned long t); // czas cyklu grzania (config.hc)

    bool isHeating() const; // czy SSR jest aktualnie załączony
    void stopHeating()
    {
        setRatio(0.0f);
        digitalWrite(SSR_PIN, LOW);
        hON = false;
        energyMeter.stopMeasurement(); // Stop measuring energy usage
    }

private:
    EnergyUsageMeter &energyMeter; // obiekt do pomiaru zużycia energii
    float ratio = 0.0f;
    unsigned long cycleTime = 10000; // domyślnie np. 10s
    unsigned long hCycleStartTime = 0;
    // bool enabled = false;         // czy grzanie jest włączone
    bool hON = false; // czy grzanie jest aktualnie włączone
};