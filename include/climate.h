#ifndef ARDUINO_MQ_CLIMATE_H
#define ARDUINO_MQ_CLIMATE_H

#include <Arduino.h>

struct ClimateReading {
  bool available;
  bool valid;
  int16_t temperatureC10;
  uint16_t humidityPct10;
};

void climateBegin();
void climatePoll(unsigned long nowMs);
ClimateReading climateReading();

#endif  // ARDUINO_MQ_CLIMATE_H

