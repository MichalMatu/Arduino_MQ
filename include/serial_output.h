#ifndef ARDUINO_MQ_SERIAL_OUTPUT_H
#define ARDUINO_MQ_SERIAL_OUTPUT_H

#include <Arduino.h>

void serialOutputBegin();
void serialOutputHeader();
void serialOutputCsv(unsigned long nowMs);

#endif  // ARDUINO_MQ_SERIAL_OUTPUT_H

