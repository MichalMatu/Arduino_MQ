#include <Arduino.h>

#include "air_quality.h"
#include "climate.h"
#include "config.h"
#include "display.h"
#include "sensors.h"
#include "serial_output.h"

namespace {

unsigned long lastSampleMs = 0UL;
unsigned long lastDisplayMs = 0UL;

bool intervalElapsed(unsigned long nowMs, unsigned long previousMs, unsigned long intervalMs) {
  return nowMs - previousMs >= intervalMs;
}

}  // namespace

void setup() {
  serialOutputBegin();
  delay(500);

  sensorsBegin();
  airQualityBegin();
  climateBegin();
  displayBegin();
  displaySplash();
  serialOutputHeader();

  sensorsUpdate();
  airQualityUpdate();
  const unsigned long startMs = millis();
  serialOutputCsv(startMs);
  lastSampleMs = startMs;
  lastDisplayMs = startMs;

  delay(Config::kSplashMs);
}

void loop() {
  const unsigned long nowMs = millis();

  climatePoll(nowMs);

  if (intervalElapsed(nowMs, lastSampleMs, Config::kSampleIntervalMs)) {
    lastSampleMs = nowMs;
    sensorsUpdate();
    airQualityUpdate();
    serialOutputCsv(nowMs);
  }

  if (intervalElapsed(nowMs, lastDisplayMs, Config::kDisplayIntervalMs)) {
    lastDisplayMs = nowMs;
    displayReadings(nowMs);
  }
}
