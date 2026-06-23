#include "sensors.h"

#include <avr/pgmspace.h>
#include <string.h>

#include "config.h"

namespace {

constexpr uint8_t kSensorPins[kSensorCount] = {
    Config::kPinMq2,
    Config::kPinMq7,
    Config::kPinMq9,
};

SensorReading readings[kSensorCount] = {};

const char kLabelMq2[] PROGMEM = "MQ-2";
const char kLabelMq7[] PROGMEM = "MQ-7";
const char kLabelMq9[] PROGMEM = "MQ-9";

const char* const kSensorLabels[] PROGMEM = {
    kLabelMq2,
    kLabelMq7,
    kLabelMq9,
};

uint8_t indexOf(SensorId id) {
  const uint8_t index = static_cast<uint8_t>(id);
  return index < kSensorCount ? index : 0U;
}

uint16_t readAveragedAnalog(uint8_t pin) {
  uint32_t sum = 0UL;

  analogRead(pin);  // Discard the first conversion after an ADC mux change.
  for (uint8_t i = 0; i < Config::kAnalogSamples; ++i) {
    sum += analogRead(pin);
    delayMicroseconds(Config::kAnalogSampleDelayUs);
  }

  return static_cast<uint16_t>((sum + (Config::kAnalogSamples / 2U)) / Config::kAnalogSamples);
}

}  // namespace

void sensorsBegin() {
  analogReference(DEFAULT);

  for (uint8_t i = 0; i < kSensorCount; ++i) {
    pinMode(kSensorPins[i], INPUT);
  }
}

void sensorsUpdate() {
  for (uint8_t i = 0; i < kSensorCount; ++i) {
    readings[i].raw = readAveragedAnalog(kSensorPins[i]);
    readings[i].millivolts = rawToMillivolts(readings[i].raw);
  }
}

SensorReading sensorReading(SensorId id) {
  return readings[indexOf(id)];
}

void copySensorLabel(SensorId id, char* buffer, size_t size) {
  if (buffer == nullptr || size == 0U) {
    return;
  }

  const uint8_t index = indexOf(id);
  PGM_P label = reinterpret_cast<PGM_P>(pgm_read_ptr(&kSensorLabels[index]));
  strncpy_P(buffer, label, size);
  buffer[size - 1U] = '\0';
}

uint16_t rawToMillivolts(uint16_t raw) {
  return static_cast<uint16_t>(
      (static_cast<uint32_t>(raw) * Config::kVrefMv) / Config::kAdcMax);
}
