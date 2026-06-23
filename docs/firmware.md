# Firmware Architecture

The v1 firmware is split into small modules to keep ownership clear and SRAM use low on ATmega328P.

## Modules

| Path | Responsibility |
|---|---|
| `include/config.h` | Pins, ADC constants, intervals, and serial speed. |
| `src/sensors.cpp` | MQ analog reads, short averaging, RAW to millivolts conversion, sensor labels. |
| `src/climate.cpp` | Lightweight AHT10 I2C driver using `Wire`, fixed-point temperature and humidity readings. |
| `src/air_quality.cpp` | Temperature/humidity compensation, 0-100 MQ index scoring, and alert state selection. |
| `src/display.cpp` | ST7920 LCD setup and page-buffer drawing with U8g2. |
| `src/serial_output.cpp` | CSV header and CSV sample output. |
| `src/main.cpp` | Startup sequence and non-blocking timing loop. |

## Resource Rules

- No `String` and no dynamic allocation.
- U8g2 uses `U8G2_ST7920_128X64_1_SW_SPI`, the page-buffer variant for Arduino Uno SRAM.
- LCD fixed strings are printed from flash with `F()`.
- Sensor labels are stored in `PROGMEM`.
- ADC samples use a short bounded loop. With three sensors and 8 samples each, the sample pass stays short enough for the one-second UI cadence.
- AHT10 measurements are polled as a small state machine. The firmware starts a conversion and reads it after the sensor delay instead of blocking the main loop for the full conversion time.
- `millis()` interval checks use unsigned subtraction, so rollover is handled by normal Arduino timing practice.

## Data Model

Each sensor reading contains:

- `raw` - ADC value from 0 to 1023.
- `millivolts` - computed as `raw * 5000 / 1023`.

No ppm conversion is included. MQ-7 and MQ-9 modules need heater cycling for calibrated gas concentration readings, and the common 4-pin modules in this project expose only module-level VCC/GND/AO/DO wiring.

The AHT10 reading contains:

- `temperatureC10` - temperature in tenths of a degree Celsius.
- `humidityPct10` - relative humidity in tenths of a percent.

Serial output prints temperature and humidity with one decimal place when `aht10_ok` is `1`.

## Alert Model

The firmware avoids ppm estimates because the MQ modules are not calibrated and MQ-7/MQ-9 heater cycling is not under firmware control.

Instead, each MQ channel is converted to a local 0-100 index:

1. Read RAW ADC and millivolts.
2. Apply a conservative temperature/humidity correction using AHT10 data.
3. Smooth the corrected RAW value with a short moving filter.
4. Scale the corrected RAW value between a per-sensor quiet point and a per-sensor full-scale point.

This means a stable but bad reading stays high on the LCD instead of being hidden by a moving baseline. The index is still not ppm and not percent LEL.

Alert fields:

- `alert` - `ok`, `ventilate`, or `alarm`.
- `alert_cause` - `none`, `mq2`, `mq7`, `mq9`, or `multiple`.
- `mq2_index`, `mq7_index`, `mq9_index` - display indices from 0 to 100.
- `mq2_comp_raw`, `mq7_comp_raw`, `mq9_comp_raw` - temperature/humidity-compensated RAW ADC values.

Default thresholds live in `include/config.h`:

- ventilation: index `35` or higher;
- alarm: index `70` or higher;
- full scale: index `100`;
- MQ-2 quiet/full corrected RAW: `380` / `700`;
- MQ-7 quiet/full corrected RAW: `70` / `420`;
- MQ-9 quiet/full corrected RAW: `70` / `500`.

These thresholds are intentionally field-tunable. Use serial logs from normal cooking, window-open ventilation, and controlled safe tests to tune them for the actual kitchen.

## LCD Dashboard

The LCD has one runtime screen:

- top row: large rounded `xxC|xx%` temperature and humidity;
- bottom row: three fields for `GAZ`, `CO`, and `AIR`;
- normal cycle: values stay visible, with `MQ2`, `MQ7`, and `MQ9` labels shown briefly once per minute;
- warning state: the affected field blinks with inverted colors;
- trend state: fast rise draws an up arrow, fast fall draws a down arrow.
