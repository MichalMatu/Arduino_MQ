#include "serial_output.h"

#include "air_quality.h"
#include "climate.h"
#include "config.h"
#include "sensors.h"

namespace {

void printSignedTenths(int16_t value) {
  const bool negative = value < 0;
  const uint16_t absolute = negative ? static_cast<uint16_t>(-value) : value;

  if (negative) {
    Serial.print('-');
  }

  Serial.print(absolute / 10U);
  Serial.print('.');
  Serial.print(absolute % 10U);
}

void printUnsignedTenths(uint16_t value) {
  Serial.print(value / 10U);
  Serial.print('.');
  Serial.print(value % 10U);
}

const __FlashStringHelper* alertLevelText(AlertLevel level) {
  switch (level) {
    case AlertLevel::kOk:
      return F("ok");
    case AlertLevel::kVentilate:
      return F("ventilate");
    case AlertLevel::kAlarm:
      return F("alarm");
  }

  return F("ok");
}

const __FlashStringHelper* alertCauseText(AlertCause cause) {
  switch (cause) {
    case AlertCause::kNone:
      return F("none");
    case AlertCause::kMq2:
      return F("mq2");
    case AlertCause::kMq7:
      return F("mq7");
    case AlertCause::kMq9:
      return F("mq9");
    case AlertCause::kMultiple:
      return F("multiple");
  }

  return F("none");
}

}  // namespace

void serialOutputBegin() {
  Serial.begin(Config::kSerialBaud);
}

void serialOutputHeader() {
  Serial.println(F(
      "time_ms,aht10_ok,temp_c,humidity_pct,alert,alert_cause,mq2_index,mq7_index,mq9_index,mq2_comp_raw,mq7_comp_raw,mq9_comp_raw,mq2_raw,mq2_mv,mq7_raw,mq7_mv,mq9_raw,mq9_mv"));
}

void serialOutputCsv(unsigned long nowMs) {
  const AirQualityStatus airStatus = airQualityStatus();
  const ClimateReading climate = climateReading();
  const SensorReading mq2 = sensorReading(SensorId::kMq2);
  const SensorReading mq7 = sensorReading(SensorId::kMq7);
  const SensorReading mq9 = sensorReading(SensorId::kMq9);

  Serial.print(nowMs);
  Serial.print(',');
  Serial.print(climate.valid ? 1 : 0);
  Serial.print(',');
  if (climate.valid) {
    printSignedTenths(climate.temperatureC10);
  }
  Serial.print(',');
  if (climate.valid) {
    printUnsignedTenths(climate.humidityPct10);
  }
  Serial.print(',');
  Serial.print(alertLevelText(airStatus.level));
  Serial.print(',');
  Serial.print(alertCauseText(airStatus.cause));
  Serial.print(',');
  Serial.print(airStatus.mq2Index);
  Serial.print(',');
  Serial.print(airStatus.mq7Index);
  Serial.print(',');
  Serial.print(airStatus.mq9Index);
  Serial.print(',');
  Serial.print(airStatus.mq2CompensatedRaw);
  Serial.print(',');
  Serial.print(airStatus.mq7CompensatedRaw);
  Serial.print(',');
  Serial.print(airStatus.mq9CompensatedRaw);
  Serial.print(',');
  Serial.print(mq2.raw);
  Serial.print(',');
  Serial.print(mq2.millivolts);
  Serial.print(',');
  Serial.print(mq7.raw);
  Serial.print(',');
  Serial.print(mq7.millivolts);
  Serial.print(',');
  Serial.print(mq9.raw);
  Serial.print(',');
  Serial.println(mq9.millivolts);
}
