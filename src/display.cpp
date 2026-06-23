#include "display.h"

#include <U8g2lib.h>

#include "air_quality.h"
#include "climate.h"
#include "config.h"

namespace {

U8G2_ST7920_128X64_1_SW_SPI u8g2(
    U8G2_R0,
    Config::kPinLcdE,
    Config::kPinLcdRw,
    Config::kPinLcdRs,
    Config::kPinLcdRst);

void drawFlashText(uint8_t x, uint8_t y, const __FlashStringHelper* text) {
  u8g2.setCursor(x, y);
  u8g2.print(text);
}

int16_t roundSignedTenths(int16_t value) {
  if (value >= 0) {
    return static_cast<int16_t>((value + 5) / 10);
  }

  return static_cast<int16_t>((value - 5) / 10);
}

uint8_t roundHumidityPct(uint16_t humidityPct10) {
  const uint16_t rounded = static_cast<uint16_t>((humidityPct10 + 5U) / 10U);
  return rounded > 99U ? 99U : static_cast<uint8_t>(rounded);
}

uint8_t clampDisplayValue(uint8_t value) {
  return value > Config::kAirIndexMax ? Config::kAirIndexMax : value;
}

void drawCelsiusMark(u8g2_uint_t x, u8g2_uint_t y) {
  u8g2.drawBox(x + 3U, y, 11U, 3U);
  u8g2.drawBox(x, y + 3U, 3U, 25U);
  u8g2.drawBox(x + 3U, y + 28U, 11U, 3U);
}

void drawSeparator(u8g2_uint_t x, u8g2_uint_t y) {
  u8g2.drawBox(x + 1U, y, 3U, 34U);
}

void drawPercentMark(u8g2_uint_t x, u8g2_uint_t y) {
  u8g2.drawBox(x, y, 6U, 6U);
  u8g2.drawBox(x + 10U, y + 24U, 6U, 6U);
  u8g2.drawLine(x + 15U, y, x, y + 30U);
  u8g2.drawLine(x + 16U, y, x + 1U, y + 30U);
}

void drawMinusMark(u8g2_uint_t x, u8g2_uint_t y) {
  u8g2.drawBox(x, y + 16U, 10U, 3U);
}

void drawClimateHeader(const ClimateReading& climate) {
  char tempDigits[5];
  char humidityDigits[4];

  if (!climate.valid) {
    u8g2.setFont(u8g2_font_6x10_tf);
    if (climate.available) {
      drawFlashText(0, 17, F("AHT10 measuring"));
    } else {
      drawFlashText(0, 17, F("AHT10 not found"));
    }
    u8g2.setFont(u8g2_font_5x7_tf);
    drawFlashText(0, 31, F("SDA=A4 SCL=A5"));
    return;
  }

  int16_t tempC = roundSignedTenths(climate.temperatureC10);
  if (tempC > 99) {
    tempC = 99;
  } else if (tempC < -9) {
    tempC = -9;
  }

  const uint16_t tempAbs = tempC < 0 ? static_cast<uint16_t>(-tempC) : tempC;
  const uint8_t humidityPct = roundHumidityPct(climate.humidityPct10);

  snprintf(tempDigits, sizeof(tempDigits), "%u", tempAbs);
  snprintf(humidityDigits, sizeof(humidityDigits), "%u", humidityPct);

  u8g2.setFont(u8g2_font_logisoso30_tn);
  const bool negative = tempC < 0;
  const u8g2_uint_t tempWidth = u8g2.getStrWidth(tempDigits);
  const u8g2_uint_t humidityWidth = u8g2.getStrWidth(humidityDigits);
  const u8g2_uint_t totalWidth =
      (negative ? 12U : 0U) +
      tempWidth + 3U + 14U + 4U + 5U + 4U + humidityWidth + 3U + 17U;

  u8g2_uint_t x = totalWidth < 128U ? (128U - totalWidth) / 2U : 0U;
  constexpr u8g2_uint_t kTop = 0U;
  constexpr u8g2_uint_t kBaseline = 34U;

  if (negative) {
    drawMinusMark(x, kTop);
    x += 12U;
  }

  u8g2.drawStr(x, kBaseline, tempDigits);
  x += tempWidth + 3U;
  drawCelsiusMark(x, kTop + 2U);
  x += 18U;
  drawSeparator(x, kTop);
  x += 9U;
  u8g2.drawStr(x, kBaseline, humidityDigits);
  x += humidityWidth + 3U;
  drawPercentMark(x, kTop + 3U);
}

void drawTrendArrow(u8g2_uint_t x, u8g2_uint_t y, TrendDirection trend) {
  if (trend == TrendDirection::kUp) {
    u8g2.drawLine(x + 3U, y, x + 3U, y + 9U);
    u8g2.drawLine(x + 3U, y, x, y + 3U);
    u8g2.drawLine(x + 3U, y, x + 6U, y + 3U);
  } else if (trend == TrendDirection::kDown) {
    u8g2.drawLine(x + 3U, y, x + 3U, y + 9U);
    u8g2.drawLine(x + 3U, y + 9U, x, y + 6U);
    u8g2.drawLine(x + 3U, y + 9U, x + 6U, y + 6U);
  }
}

void drawMetricField(
    u8g2_uint_t x,
    u8g2_uint_t width,
    const __FlashStringHelper* label,
    uint8_t level,
    TrendDirection trend,
    bool showLabel,
    unsigned long nowMs) {
  constexpr u8g2_uint_t kY = 40U;
  constexpr u8g2_uint_t kHeight = 24U;
  char value[4];
  const bool alert = level >= Config::kAirVentilateIndex;
  const bool inverted = alert && (((nowMs / 500UL) & 1UL) == 0UL);

  if (inverted) {
    u8g2.drawBox(x, kY, width, kHeight);
    u8g2.setDrawColor(0);
  } else {
    u8g2.drawFrame(x, kY, width, kHeight);
    u8g2.setDrawColor(1);
  }

  if (showLabel) {
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setCursor(x + 6U, kY + 16U);
    u8g2.print(label);
  } else {
    snprintf(value, sizeof(value), "%u", clampDisplayValue(level));
    u8g2.setFont(level >= 100U ? u8g2_font_7x13B_tf : u8g2_font_9x15B_tf);
    const u8g2_uint_t valueWidth = u8g2.getStrWidth(value);
    u8g2.drawStr(x + ((width - valueWidth) / 2U), kY + 17U, value);
    if (level < Config::kAirIndexMax) {
      drawTrendArrow(x + width - 10U, kY + 7U, trend);
    }
  }

  u8g2.setDrawColor(1);
}

void drawDashboard(unsigned long nowMs) {
  const ClimateReading climate = climateReading();
  const AirQualityStatus airStatus = airQualityStatus();
  const bool showLabels = (nowMs % 60000UL) >= 59000UL;

  u8g2.firstPage();
  do {
    drawClimateHeader(climate);
    drawMetricField(
        0, 41, F("MQ2"), airStatus.mq2Index, airStatus.mq2Trend, showLabels, nowMs);
    drawMetricField(
        43, 41, F("MQ7"), airStatus.mq7Index, airStatus.mq7Trend, showLabels, nowMs);
    drawMetricField(
        86, 42, F("MQ9"), airStatus.mq9Index, airStatus.mq9Trend, showLabels, nowMs);
  } while (u8g2.nextPage());
}

}  // namespace

void displayBegin() {
  u8g2.begin();
}

void displaySplash() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_6x10_tf);
    drawFlashText(0, 12, F("Arduino Uno + MQ"));
    drawFlashText(0, 28, F("AHT10 climate"));
    u8g2.setFont(u8g2_font_5x7_tf);
    drawFlashText(0, 48, F("Check PSB=GND"));
    drawFlashText(0, 60, F("SDA=A4 SCL=A5"));
  } while (u8g2.nextPage());
}

void displayReadings(unsigned long nowMs) {
  drawDashboard(nowMs);
}
