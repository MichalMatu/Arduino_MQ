#include "air_quality.h"

#include "climate.h"
#include "config.h"
#include "sensors.h"

namespace {

struct ChannelTracker {
  bool initialized;
  uint16_t filteredRaw;
  uint8_t previousIndex;
};

ChannelTracker trackers[kSensorCount] = {};
AirQualityStatus status = {
    AlertLevel::kOk,
    AlertCause::kNone,
    0,
    0,
    0,
    TrendDirection::kStable,
    TrendDirection::kStable,
    TrendDirection::kStable,
    0,
    0,
    0,
};

bool trendInitialized = false;

uint8_t indexOf(SensorId id) {
  return static_cast<uint8_t>(id);
}

int16_t clampPermille(int16_t value) {
  if (value < Config::kCorrectionMinPermille) {
    return Config::kCorrectionMinPermille;
  }

  if (value > Config::kCorrectionMaxPermille) {
    return Config::kCorrectionMaxPermille;
  }

  return value;
}

uint16_t compensateRaw(uint16_t raw, const ClimateReading& climate) {
  if (!climate.valid) {
    return raw;
  }

  const int16_t tempDeltaC =
      static_cast<int16_t>((climate.temperatureC10 - Config::kClimateRefTempC10) / 10);
  const int16_t humidityDeltaPct = static_cast<int16_t>(
      (static_cast<int16_t>(climate.humidityPct10) -
       static_cast<int16_t>(Config::kClimateRefHumidityPct10)) /
      10);

  const int16_t factorPermille = clampPermille(static_cast<int16_t>(
      1000 +
      (tempDeltaC * Config::kTempCorrectionPermillePerC) +
      (humidityDeltaPct * Config::kHumidityCorrectionPermillePerPct)));

  return static_cast<uint16_t>(
      (static_cast<uint32_t>(raw) * 1000UL + (factorPermille / 2)) /
      static_cast<uint16_t>(factorPermille));
}

uint16_t filteredRaw(ChannelTracker* tracker, uint16_t compensatedRaw) {
  if (!tracker->initialized) {
    tracker->initialized = true;
    tracker->filteredRaw = compensatedRaw;
    tracker->previousIndex = 0U;
    return compensatedRaw;
  }

  tracker->filteredRaw = static_cast<uint16_t>(
      ((static_cast<uint32_t>(tracker->filteredRaw) * 3UL) + compensatedRaw + 2UL) /
      4UL);
  return tracker->filteredRaw;
}

uint8_t scaledIndex(uint16_t raw, uint16_t zeroRaw, uint16_t fullRaw) {
  if (raw <= zeroRaw || fullRaw <= zeroRaw) {
    return 0U;
  }

  const uint32_t span = static_cast<uint32_t>(fullRaw - zeroRaw);
  uint32_t scaled =
      (static_cast<uint32_t>(raw - zeroRaw) * Config::kAirIndexMax + (span / 2UL)) /
      span;

  if (scaled > Config::kAirIndexMax) {
    scaled = Config::kAirIndexMax;
  }

  return static_cast<uint8_t>(scaled);
}

TrendDirection trendFor(uint8_t current, uint8_t previous) {
  constexpr uint8_t kTrendStep = 3U;

  if (current >= previous + kTrendStep) {
    return TrendDirection::kUp;
  }

  if (previous >= current + kTrendStep) {
    return TrendDirection::kDown;
  }

  return TrendDirection::kStable;
}

void markCause(bool mq2Active, bool mq7Active, bool mq9Active) {
  const uint8_t activeCount =
      (mq2Active ? 1U : 0U) + (mq7Active ? 1U : 0U) + (mq9Active ? 1U : 0U);

  if (activeCount == 0U) {
    status.cause = AlertCause::kNone;
  } else if (activeCount > 1U) {
    status.cause = AlertCause::kMultiple;
  } else if (mq2Active) {
    status.cause = AlertCause::kMq2;
  } else if (mq7Active) {
    status.cause = AlertCause::kMq7;
  } else {
    status.cause = AlertCause::kMq9;
  }
}

}  // namespace

void airQualityBegin() {
  for (uint8_t i = 0; i < kSensorCount; ++i) {
    trackers[i] = {false, 0U, 0U};
  }

  trendInitialized = false;
}

void airQualityUpdate() {
  const SensorReading mq2 = sensorReading(SensorId::kMq2);
  const SensorReading mq7 = sensorReading(SensorId::kMq7);
  const SensorReading mq9 = sensorReading(SensorId::kMq9);
  const ClimateReading climate = climateReading();

  status.mq2CompensatedRaw = compensateRaw(mq2.raw, climate);
  status.mq7CompensatedRaw = compensateRaw(mq7.raw, climate);
  status.mq9CompensatedRaw = compensateRaw(mq9.raw, climate);

  const uint16_t mq2Filtered =
      filteredRaw(&trackers[indexOf(SensorId::kMq2)], status.mq2CompensatedRaw);
  const uint16_t mq7Filtered =
      filteredRaw(&trackers[indexOf(SensorId::kMq7)], status.mq7CompensatedRaw);
  const uint16_t mq9Filtered =
      filteredRaw(&trackers[indexOf(SensorId::kMq9)], status.mq9CompensatedRaw);

  const uint8_t mq2Index =
      scaledIndex(mq2Filtered, Config::kMq2IndexZeroRaw, Config::kMq2IndexFullRaw);
  const uint8_t mq7Index =
      scaledIndex(mq7Filtered, Config::kMq7IndexZeroRaw, Config::kMq7IndexFullRaw);
  const uint8_t mq9Index =
      scaledIndex(mq9Filtered, Config::kMq9IndexZeroRaw, Config::kMq9IndexFullRaw);

  if (trendInitialized) {
    status.mq2Trend = trendFor(mq2Index, trackers[indexOf(SensorId::kMq2)].previousIndex);
    status.mq7Trend = trendFor(mq7Index, trackers[indexOf(SensorId::kMq7)].previousIndex);
    status.mq9Trend = trendFor(mq9Index, trackers[indexOf(SensorId::kMq9)].previousIndex);
  } else {
    status.mq2Trend = TrendDirection::kStable;
    status.mq7Trend = TrendDirection::kStable;
    status.mq9Trend = TrendDirection::kStable;
    trendInitialized = true;
  }

  status.mq2Index = mq2Index;
  status.mq7Index = mq7Index;
  status.mq9Index = mq9Index;

  trackers[indexOf(SensorId::kMq2)].previousIndex = mq2Index;
  trackers[indexOf(SensorId::kMq7)].previousIndex = mq7Index;
  trackers[indexOf(SensorId::kMq9)].previousIndex = mq9Index;

  const bool mq2Alarm = mq2Index >= Config::kAirAlarmIndex;
  const bool mq7Alarm = mq7Index >= Config::kAirAlarmIndex;
  const bool mq9Alarm = mq9Index >= Config::kAirAlarmIndex;
  const bool mq2Ventilate = mq2Index >= Config::kAirVentilateIndex;
  const bool mq7Ventilate = mq7Index >= Config::kAirVentilateIndex;
  const bool mq9Ventilate = mq9Index >= Config::kAirVentilateIndex;

  if (mq2Alarm || mq7Alarm || mq9Alarm) {
    status.level = AlertLevel::kAlarm;
    markCause(mq2Alarm, mq7Alarm, mq9Alarm);
  } else if (mq2Ventilate || mq7Ventilate || mq9Ventilate) {
    status.level = AlertLevel::kVentilate;
    markCause(mq2Ventilate, mq7Ventilate, mq9Ventilate);
  } else {
    status.level = AlertLevel::kOk;
    status.cause = AlertCause::kNone;
  }
}

AirQualityStatus airQualityStatus() {
  return status;
}
