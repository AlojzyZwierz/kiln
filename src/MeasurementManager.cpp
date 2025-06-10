#include "MeasurementManager.h"
#include <cmath>
#include <Arduino.h>
void MeasurementManager::addMeasurement(unsigned long time, float temp)
{
  measurements.emplace_back((uint16_t)(time / 1000), (uint16_t)(std::round(temp)));
  // Serial.println("Measurement added: " + String(time) + " " + String(temp) + " size " + String(measurements.size()) +" " "Measurements no: " + String(MeasurementManager::get().getMeasurements().size()));
}

const std::vector<Measurement> &MeasurementManager::getMeasurements() const
{
  return measurements;
}

void MeasurementManager::clear()
{
  measurements.clear();
}
