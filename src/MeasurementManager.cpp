#include "MeasurementManager.h"
#include <cmath>
#include <Arduino.h>
void MeasurementManager::addMeasurement()
{
  //(millis() - ProcessController::get().getProgramStartTime(), temperatureSensor.getTemperature());
  measurements.emplace_back((uint16_t)((millis() - ProcessController::get().getProgramStartTime()) / 1000), (uint16_t)(std::round(ProcessController::get().getCurrentTemp())));
  // Serial.println("Measurement added: " + String(time) + " " + String(temp) + " size " + String(measurements.size()) +" " "Measurements no: " + String(MeasurementManager::get().getMeasurements().size()));
  setNextMeasurementTime(millis() + measurementInterval);
}

const std::vector<Measurement> &MeasurementManager::getMeasurements() const
{
  return measurements;
}

void MeasurementManager::clear()
{
  measurements.clear();
}
