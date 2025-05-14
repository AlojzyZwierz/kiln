#ifndef FAKE_FURNACE_H
#define FAKE_FURNACE_H

class FakeFurnace {
public:
    void begin();
    void update();
    void setHeatingPower(float power); // Zakres: 0.0 - 1.0
    float getTemperature() const;

private:
    float temperature = 25.0f;
    float storedHeat = 0.0f;
    float heatingPower = 0.0f;

    unsigned long lastUpdate = 0;

    float computeCoolingLoss(float temp);
};

#endif
