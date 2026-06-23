#ifndef ARDUINO_MQ_CONFIG_H
#define ARDUINO_MQ_CONFIG_H

#include <Arduino.h>

namespace Config {

constexpr unsigned long kSerialBaud = 115200UL;

constexpr uint8_t kPinLcdE = 13U;    // LCD pin 6 E / SCLK.
constexpr uint8_t kPinLcdRw = 11U;   // LCD pin 5 R/W / SID.
constexpr uint8_t kPinLcdRs = 10U;   // LCD pin 4 RS / CS.
constexpr uint8_t kPinLcdRst = 8U;   // LCD pin 17 RST.

constexpr uint8_t kPinMq2 = A0;
constexpr uint8_t kPinMq7 = A1;
constexpr uint8_t kPinMq9 = A2;

constexpr uint8_t kAht10Address = 0x38U;

constexpr uint16_t kAdcMax = 1023U;
constexpr uint16_t kVrefMv = 5000U;
constexpr uint8_t kAnalogSamples = 8U;
constexpr uint16_t kAnalogSampleDelayUs = 1500U;

constexpr uint8_t kAirVentilateIndex = 35U;
constexpr uint8_t kAirAlarmIndex = 70U;
constexpr uint8_t kAirIndexMax = 100U;

constexpr uint16_t kMq2IndexZeroRaw = 380U;
constexpr uint16_t kMq2IndexFullRaw = 700U;
constexpr uint16_t kMq7IndexZeroRaw = 70U;
constexpr uint16_t kMq7IndexFullRaw = 420U;
constexpr uint16_t kMq9IndexZeroRaw = 70U;
constexpr uint16_t kMq9IndexFullRaw = 500U;

constexpr int16_t kClimateRefTempC10 = 250;
constexpr uint16_t kClimateRefHumidityPct10 = 500U;
constexpr int16_t kTempCorrectionPermillePerC = 3;
constexpr int16_t kHumidityCorrectionPermillePerPct = 2;
constexpr int16_t kCorrectionMinPermille = 850;
constexpr int16_t kCorrectionMaxPermille = 1150;

constexpr unsigned long kSampleIntervalMs = 1000UL;
constexpr unsigned long kDisplayIntervalMs = 500UL;
constexpr unsigned long kClimateIntervalMs = 2000UL;
constexpr unsigned long kClimateRetryIntervalMs = 5000UL;
constexpr unsigned long kAht10MeasurementDelayMs = 85UL;
constexpr unsigned long kSplashMs = 1200UL;

}  // namespace Config

#endif  // ARDUINO_MQ_CONFIG_H
