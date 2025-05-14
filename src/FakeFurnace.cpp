#include "FakeFurnace.h"
#include <Arduino.h>
#include <math.h>

void FakeFurnace::begin() {
    lastUpdate = millis();
    temperature = 25.0f;
    storedHeat = 0.0f;
}

void FakeFurnace::setHeatingPower(float power) {
    heatingPower = constrain(power, 0.0f, 1.0f);
    Serial.println("power set: " + String(power));
}

float FakeFurnace::getTemperature() const {
    
    Serial.println("temp sent: " + String(temperature));
    return temperature;
}

void FakeFurnace::update() {

    unsigned long now = millis();
    if (now < lastUpdate + 1000) {
        return;
    }
    float dt = (now - lastUpdate) / 1000.0f; // w sekundach
    lastUpdate = now;

    if (dt <= 0.0f) return;

    // Moc grzania dodaje ciepło
    storedHeat += heatingPower * 15.0f * dt;

    // Utrata ciepła (więcej przy wyższych temp)
    float cooling = 0.1f*computeCoolingLoss(temperature) * dt;
    storedHeat -= cooling;
    Serial.println("Stored heat: " + String(storedHeat) + " Cooling: " + String(cooling) + " dt: " + String(dt) + " Power: " + String(heatingPower)+ " Temp: " + String(temperature) );
    if (storedHeat < 0.0f) storedHeat = 0.0f;

    // Nowa temperatura (proporcjonalna, asymptota w okolicach 1300°C)
    temperature += (storedHeat) * 0.2f; // 0.1f to współczynnik konwersji ciepła na temperaturę
    storedHeat -= storedHeat * dt * 1.0f; // Utrata ciepła w czasie
    if (temperature > 1350.0f) temperature = 1350.0f;
    
}

float FakeFurnace::computeCoolingLoss(float temp) {
    if (temp <= 25.0f) return 0.0f;
    return (temp - 25.0f) * 0.01f + pow((temp - 25.0f) / 100.0f, 2.0f); // prosty model strat
}
