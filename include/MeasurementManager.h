#ifndef MEASUREMENTMANAGER_H
#define MEASUREMENTMANAGER_H

#include <vector>

#include <cstdint>
#include "ProcessController.h"

struct Measurement
{
  uint16_t time;
  uint16_t temp;
  // char code;

  Measurement(uint16_t t, uint16_t tmp)
      : time(t), temp(tmp) {}
};

class MeasurementManager
{
public:
  static MeasurementManager &get()
  {
    static MeasurementManager instance;
    return instance;
  }
  void addMeasurement();
  const std::vector<Measurement> &getMeasurements() const;
  void clear();
  MeasurementManager(const MeasurementManager &) = delete;
  void operator=(const MeasurementManager &) = delete;
  unsigned long getMeasurementInterval() const { return measurementInterval; }
  void setMeasurementInterval(unsigned long interval) { measurementInterval = interval; }
  unsigned long getNextMeasurementTime() const { return nextMeasurementTime; }
  void setNextMeasurementTime(unsigned long time) { nextMeasurementTime = time; }

private:
  MeasurementManager() {}
  std::vector<Measurement> measurements;
  unsigned long measurementInterval = 150000;// 2.5 min
  unsigned long nextMeasurementTime = 0;
};

#endif // MEASUREMENTMANAGER_H
