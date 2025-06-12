#include "HeatingController.h"

void HeatingController::update()
{
    unsigned long now = millis();
    unsigned long hOnInCycle = constrain(ratio, 0.0f, 1.0f) * cycleTime;
    unsigned long timeFromCycleStart = now - hCycleStartTime;
    if (SystemState::get().getMode() == SystemMode::Firing)
    {
        if (hOnInCycle > timeFromCycleStart)
        {
            digitalWrite(SSR_PIN, HIGH);
            hON = true;
            energyMeter.startMeasurement(); // Start measuring energy usage
            // Serial.println("Heating ON, cycle time: " + String(cycleTime) + ", hOnInCycle: " + String(hOnInCycle) + ", timeFromCycleStart: " + String(timeFromCycleStart));
        }
        else
        {
            digitalWrite(SSR_PIN, LOW);
            hON = false;
            energyMeter.stopMeasurement(); // Stop measuring energy usage
            // Serial.println("Heating OFF, cycle time: " + String(cycleTime) + ", hOnInCycle: " + String(hOnInCycle) + ", timeFromCycleStart: " + String(timeFromCycleStart));
        }

        if (timeFromCycleStart >= cycleTime)
        {
            hCycleStartTime = now;
        }
    }
    else
    {
        digitalWrite(SSR_PIN, LOW);
        hON = false;
    }
}

void HeatingController::setRatio(float r)
{
    ratio = constrain(r, 0.0f, 1.0f);
}
/*
void HeatingController::setEnabled(bool on) {
    enabled = on;
    if(!on) {
        digitalWrite(SSR, LOW);
        hON = false;
    }
}
*/
void HeatingController::setCycleTime(unsigned long t)
{
    cycleTime = t;
}

bool HeatingController::isHeating() const
{
    return hON;
}
float HeatingController::getRatio()
{
    return ratio;
}