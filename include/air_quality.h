#ifndef ARDUINO_MQ_AIR_QUALITY_H
#define ARDUINO_MQ_AIR_QUALITY_H

#include <Arduino.h>

enum class AlertLevel : uint8_t {
  kOk = 0,
  kVentilate,
  kAlarm,
};

enum class AlertCause : uint8_t {
  kNone = 0,
  kMq2,
  kMq7,
  kMq9,
  kMultiple,
};

enum class TrendDirection : uint8_t {
  kStable = 0,
  kUp,
  kDown,
};

struct AirQualityStatus {
  AlertLevel level;
  AlertCause cause;
  uint8_t mq2Index;
  uint8_t mq7Index;
  uint8_t mq9Index;
  TrendDirection mq2Trend;
  TrendDirection mq7Trend;
  TrendDirection mq9Trend;
  uint16_t mq2CompensatedRaw;
  uint16_t mq7CompensatedRaw;
  uint16_t mq9CompensatedRaw;
};

void airQualityBegin();
void airQualityUpdate();
AirQualityStatus airQualityStatus();

#endif  // ARDUINO_MQ_AIR_QUALITY_H
