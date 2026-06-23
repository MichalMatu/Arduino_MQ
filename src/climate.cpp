#include "climate.h"

#include <Wire.h>

#include "config.h"

namespace {

constexpr uint8_t kCmdInitialize = 0xE1U;
constexpr uint8_t kCmdMeasure = 0xACU;
constexpr uint8_t kCmdSoftReset = 0xBAU;
constexpr uint8_t kStatusBusy = 0x80U;
constexpr uint8_t kStatusCalibrated = 0x08U;
constexpr uint8_t kMeasurementBytes = 6U;
constexpr uint32_t kAhtScale = 1048576UL;

enum class DriverState : uint8_t {
  kUnavailable,
  kIdle,
  kMeasuring,
};

ClimateReading latest = {false, false, 0, 0};
DriverState state = DriverState::kUnavailable;
unsigned long lastStartMs = 0UL;
unsigned long lastRetryMs = 0UL;

bool writeCommand(uint8_t command) {
  Wire.beginTransmission(Config::kAht10Address);
  Wire.write(command);
  return Wire.endTransmission() == 0;
}

bool writeCommand(uint8_t command, uint8_t arg0, uint8_t arg1) {
  Wire.beginTransmission(Config::kAht10Address);
  Wire.write(command);
  Wire.write(arg0);
  Wire.write(arg1);
  return Wire.endTransmission() == 0;
}

bool readStatus(uint8_t* status) {
  if (status == nullptr) {
    return false;
  }

  const uint8_t received = Wire.requestFrom(Config::kAht10Address, static_cast<uint8_t>(1U));
  if (received != 1U || Wire.available() == 0) {
    return false;
  }

  *status = Wire.read();
  return true;
}

bool initializeSensor() {
  if (!writeCommand(kCmdSoftReset)) {
    return false;
  }
  delay(20);

  if (!writeCommand(kCmdInitialize, 0x08U, 0x00U)) {
    return false;
  }
  delay(10);

  uint8_t status = 0U;
  if (!readStatus(&status)) {
    return false;
  }

  return (status & kStatusCalibrated) != 0U;
}

bool startMeasurement(unsigned long nowMs) {
  if (!writeCommand(kCmdMeasure, 0x33U, 0x00U)) {
    return false;
  }

  lastStartMs = nowMs;
  state = DriverState::kMeasuring;
  return true;
}

bool readMeasurement() {
  uint8_t data[kMeasurementBytes] = {};
  const uint8_t received = Wire.requestFrom(Config::kAht10Address, kMeasurementBytes);

  if (received != kMeasurementBytes) {
    return false;
  }

  for (uint8_t i = 0; i < kMeasurementBytes; ++i) {
    if (Wire.available() == 0) {
      return false;
    }
    data[i] = Wire.read();
  }

  if ((data[0] & kStatusBusy) != 0U) {
    return false;
  }

  const uint32_t rawHumidity =
      (static_cast<uint32_t>(data[1]) << 12U) |
      (static_cast<uint32_t>(data[2]) << 4U) |
      (static_cast<uint32_t>(data[3]) >> 4U);

  const uint32_t rawTemperature =
      ((static_cast<uint32_t>(data[3]) & 0x0FUL) << 16U) |
      (static_cast<uint32_t>(data[4]) << 8U) |
      static_cast<uint32_t>(data[5]);

  uint16_t humidityPct10 = static_cast<uint16_t>(
      ((rawHumidity * 1000UL) + (kAhtScale / 2UL)) / kAhtScale);
  if (humidityPct10 > 1000U) {
    humidityPct10 = 1000U;
  }

  const int16_t temperatureC10 = static_cast<int16_t>(
      ((rawTemperature * 2000UL) + (kAhtScale / 2UL)) / kAhtScale) -
      500;

  latest.available = true;
  latest.valid = true;
  latest.temperatureC10 = temperatureC10;
  latest.humidityPct10 = humidityPct10;
  return true;
}

void markUnavailable(unsigned long nowMs) {
  latest.available = false;
  latest.valid = false;
  state = DriverState::kUnavailable;
  lastRetryMs = nowMs;
}

}  // namespace

void climateBegin() {
  Wire.begin();
  Wire.setClock(100000UL);

  if (initializeSensor()) {
    latest.available = true;
    latest.valid = false;
    state = DriverState::kIdle;
  } else {
    markUnavailable(millis());
  }
}

void climatePoll(unsigned long nowMs) {
  switch (state) {
    case DriverState::kUnavailable:
      if (nowMs - lastRetryMs >= Config::kClimateRetryIntervalMs && initializeSensor()) {
        latest.available = true;
        latest.valid = false;
        state = DriverState::kIdle;
      }
      break;

    case DriverState::kIdle:
      if (nowMs - lastStartMs >= Config::kClimateIntervalMs && !startMeasurement(nowMs)) {
        markUnavailable(nowMs);
      }
      break;

    case DriverState::kMeasuring:
      if (nowMs - lastStartMs >= Config::kAht10MeasurementDelayMs) {
        if (readMeasurement()) {
          state = DriverState::kIdle;
        } else {
          markUnavailable(nowMs);
        }
      }
      break;
  }
}

ClimateReading climateReading() {
  return latest;
}
