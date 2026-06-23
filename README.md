# Arduino MQ Monitor

Firmware for an Arduino Uno-compatible ATmega328P board with a 128x64 LCD 12864B-V2.3, three MQ sensor modules, and an AHT10 temperature/humidity module.

The firmware reads MQ-2, MQ-7, and MQ-9 analog outputs, reads AHT10 over I2C, and shows one always-on kitchen dashboard. The bottom dashboard fields are temperature/humidity-compensated 0-100 air indices, not ppm. It does not use SIM800L, nRF24L01+, OpenLog, or serial simulation.

This is a helper indicator for a domestic natural gas kitchen. It is not a certified combustible gas or carbon monoxide alarm.

## Build

```bash
pio run
pio run -t upload
pio device monitor -b 115200
```

## Hardware v1

- Arduino Uno-compatible ATmega328P board.
- LCD 12864B-V2.3 in ST7920 serial mode.
- MQ-2 AO -> A0.
- MQ-7 AO -> A1.
- MQ-9 AO -> A2.
- AHT10 VIN -> 5V, GND -> GND, SCL -> A5, SDA -> A4.
- USB Serial Monitor at 115200 baud.

Important LCD wiring: `PSB` must be tied to `GND`, otherwise the display stays in parallel mode.

## Repository Layout

- `include/` - firmware module interfaces and compile-time configuration.
- `src/` - firmware implementation.
- `docs/pinout.md` - wiring tables.
- `docs/firmware.md` - firmware architecture and resource rules.
- `docs/hardware.md` - module inventory with real hardware photos.
- `docs/source/` - original DOCX and ZIP inputs preserved for traceability.

## CSV Output

```text
time_ms,aht10_ok,temp_c,humidity_pct,alert,alert_cause,mq2_index,mq7_index,mq9_index,mq2_comp_raw,mq7_comp_raw,mq9_comp_raw,mq2_raw,mq2_mv,mq7_raw,mq7_mv,mq9_raw,mq9_mv
```

Treat the indices as local kitchen indicators. MQ modules need time to stabilize, and MQ-7/MQ-9 need heater control for calibrated gas concentration measurements, which this v1 hardware wiring does not provide.

## Display Behavior

- The top line shows large rounded climate data: `xxC|xx%`.
- The bottom row has three fields: `GAZ`, `CO`, `AIR`.
- For 10 seconds the bottom row shows compensated values `0-100`.
- Once per minute the bottom row briefly shows sensor labels `MQ2`, `MQ7`, `MQ9`.
- A fast rise draws an up arrow; a fast fall draws a down arrow.
- A field at or above the ventilation threshold blinks with inverted colors.

The thresholds are based on temperature/humidity-compensated RAW ADC ranges. They are meant to catch unusual conditions near a gas stove, not to estimate legal or safety-standard ppm/LEL values.
