#include "TemperatureSensor.h"

TemperatureSensor::TemperatureSensor()
    : thermocouple(10, 11, 12, 13) // CS, DI, DO, CLK – zmień piny wg potrzeb
{
}

void TemperatureSensor::begin() {
    thermocouple.begin();
    thermocouple.setThermocoupleType(MAX31856_TCTYPE_S);
}

void TemperatureSensor::update() {
    float temp = thermocouple.readThermocoupleTemperature();
    uint8_t faults = thermocouple.readFault();

    if (faults == 0 && !isnan(temp)) {
        lastValidTemperature = temp;
        errorCount = 0;
    } else {
        handleError();
    }
}

void TemperatureSensor::handleError() {
    errorCount++;
    if (errorCount >= MAX_ERRORS) {
        // Tu można dodać np. flagę krytycznego błędu
    }
}

float TemperatureSensor::getTemperature() const {
    return lastValidTemperature;
}

int TemperatureSensor::getErrorCount() const {
    return errorCount;
}

bool TemperatureSensor::hasError() const {
    return errorCount > 0;
}
