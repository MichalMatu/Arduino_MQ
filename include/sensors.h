#ifndef ARDUINO_MQ_SENSORS_H
#define ARDUINO_MQ_SENSORS_H

#include <Arduino.h>

enum class SensorId : uint8_t {
  kMq2 = 0,
  kMq7,
  kMq9,
  kCount,
};

constexpr uint8_t kSensorCount = static_cast<uint8_t>(SensorId::kCount);

struct SensorReading {
  uint16_t raw;
  uint16_t millivolts;
};

void sensorsBegin();
void sensorsUpdate();
SensorReading sensorReading(SensorId id);
void copySensorLabel(SensorId id, char* buffer, size_t size);
uint16_t rawToMillivolts(uint16_t raw);

#endif  // ARDUINO_MQ_SENSORS_H

