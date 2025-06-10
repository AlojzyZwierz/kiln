#ifndef MEASUREMENTMANAGER_H
#define MEASUREMENTMANAGER_H

#include <vector>

#include <cstdint>

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
  void addMeasurement(unsigned long t, float tmp);
  const std::vector<Measurement> &getMeasurements() const;
  void clear();
  MeasurementManager(const MeasurementManager &) = delete;
  void operator=(const MeasurementManager &) = delete;

private:
  MeasurementManager() {}
  std::vector<Measurement> measurements;
};

#endif // MEASUREMENTMANAGER_H
