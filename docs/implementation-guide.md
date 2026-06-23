# Implementation Guide

Source: `source/instrukcja_implementacji_arduino_mq_lcd.docx`, prepared on 2026-06-23.

## Scope

Build firmware in VS Code + PlatformIO for an Arduino Uno-compatible ATmega328P board.

The base firmware must:

- read MQ-2, MQ-7, and MQ-9 analog outputs;
- show one dashboard with temperature/humidity and scaled `GAZ`/`CO`/`AIR` fields;
- print CSV through the USB serial monitor at 115200 baud;
- avoid `String`, heap allocation, ppm calculations, GSM, radio, and OpenLog.

The current firmware also reads an AHT10 temperature/humidity module over I2C. This is used for the local MQ index compensation, not for direct ppm conversion.

## Hardware Decisions

| Element | Identification | v1 decision |
|---|---|---|
| Arduino Uno-compatible clone | ATmega328P-AU, 16 MHz, Uno R3 pin layout | Main 5 V board. |
| LCD 12864B-V2.3 | 20-pin 128x64 LCD, ST7920-compatible assumption | Use serial mode with `PSB` tied to `GND`. |
| MQ-2 module | Flying-Fish AO/DO/VCC/GND module | AO to A0. |
| MQ-7 module | Flying-Fish AO/DO/VCC/GND module | AO to A1; RAW/trend only. |
| MQ-9 module | Flying-Fish AO/DO/VCC/GND module | AO to A2; RAW/trend only. |
| AHT10 module | I2C temperature/humidity breakout | VIN to 5V, GND to GND, SCL to A5, SDA to A4. |
| OpenLog | SparkFun-compatible serial microSD logger | Not used in v1. |
| nRF24L01+ | 2.4 GHz radio modules | Not used in v1. |
| SIM800L EVB v2.0 | GSM/GPRS module | Not used in v1. |

## Bring-up Procedure

1. Connect only the LCD and Arduino.
2. Tie LCD `PSB` to `GND`.
3. Upload firmware and confirm the splash screen appears.
4. If the LCD is blank, check `PSB`, contrast, `RST`, and the RS/RW/E pin order.
5. Connect MQ-2 AO to A0 and check serial CSV plus LCD output.
6. Connect MQ-7 AO to A1 and MQ-9 AO to A2.
7. Connect AHT10: VIN to 5V, GND to GND, SCL to A5, SDA to A4.
8. Let the MQ sensors warm up. Early readings are expected to drift.

## Expected CSV

```text
time_ms,aht10_ok,temp_c,humidity_pct,alert,alert_cause,mq2_index,mq7_index,mq9_index,mq2_comp_raw,mq7_comp_raw,mq9_comp_raw,mq2_raw,mq2_mv,mq7_raw,mq7_mv,mq9_raw,mq9_mv
```

## Kitchen Alert Behavior

The target installation is a domestic natural-gas stove. Natural gas is mostly methane and is lighter than air, so the indicator should sit above the stove area rather than near the floor.

LCD behavior:

- top row always shows large rounded `xxC|xx%` temperature/humidity;
- bottom row shows `GAZ`, `CO`, and `AIR`;
- bottom values are compensated indices from 0 to 100;
- values stay visible, with `MQ2`, `MQ7`, and `MQ9` labels shown briefly once per minute;
- fields blink with inverted colors when they cross the ventilation threshold;
- trend arrows indicate quick rise or fall.

Serial output remains detailed so thresholds can be tuned from logs.

## Typical Problems

| Symptom | Likely cause | Fix |
|---|---|---|
| Blank LCD | `PSB` not tied to `GND`, contrast issue, wrong RS/RW/E order, missing reset | Check serial-mode wiring and contrast first. |
| Garbage on LCD | Wrong ST7920 pin order or unstable wiring | Test only LCD with short wires. |
| ADC reads 0 or 1023 | Wrong AO/VCC/GND wiring or no common ground | Read the module silkscreen and use AO, not DO. |
| MQ values drift | Sensor warm-up and environmental sensitivity | Treat v1 as a trend monitor, not calibrated ppm. |
| Alert trips during normal cooking | Index range or threshold too sensitive for the kitchen | Log `mq*_index` and `mq*_comp_raw`, then adjust thresholds in `include/config.h`. |
| Alert never trips during a safe test | Threshold too high or module placement is poor | Move the device closer/higher relative to the stove and lower thresholds carefully. |
| SRAM pressure | Full LCD buffer, `String`, large globals | Use page-buffer U8g2, fixed buffers, and flash strings. |
