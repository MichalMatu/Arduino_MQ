# Hardware Inventory

The photos below are the real modules supplied with the project. The current firmware uses the Arduino Uno-compatible board, LCD 12864B-V2.3, MQ-2/MQ-7/MQ-9 modules, and an AHT10 temperature/humidity module.

## Used in v1

| Module | Photo | v1 decision |
|---|---|---|
| Arduino Uno-compatible ATmega328P board | [arduino-uno-compatible-back.jpg](hardware/photos/arduino-uno-compatible-back.jpg) | Main 5 V board. |
| LCD 12864B-V2.3 | [lcd-12864b-v23-back.jpg](hardware/photos/lcd-12864b-v23-back.jpg) | Use as ST7920-compatible 128x64 LCD in serial mode. Tie `PSB` to `GND`. |
| MQ-2 Flying-Fish module | [mq-sensors-labels.jpg](hardware/photos/mq-sensors-labels.jpg) | AO to A0. RAW/trend only. |
| MQ-7 Flying-Fish module | [mq-sensors-labels.jpg](hardware/photos/mq-sensors-labels.jpg) | AO to A1. RAW/trend only, not ppm. |
| MQ-9 Flying-Fish module | [mq-sensors-labels.jpg](hardware/photos/mq-sensors-labels.jpg) | AO to A2. RAW/trend only, not ppm. |
| AHT10 temperature/humidity module | [aht10-module.jpg](hardware/photos/aht10-module.jpg) | I2C on A4/A5. Logs temperature and humidity next to MQ readings. |

Additional MQ overview photo: [mq-sensors-front.jpg](hardware/photos/mq-sensors-front.jpg).

## Present but not used in v1

| Module | Photo | Reason |
|---|---|---|
| OpenLog microSD logger | [openlog-module.jpg](hardware/photos/openlog-module.jpg) | Later v2 candidate. Uno has one hardware UART, and OpenLog RXI should not receive direct 5 V TX without level handling. |
| nRF24L01+ radio modules | [nrf24l01-modules.jpg](hardware/photos/nrf24l01-modules.jpg) | Later v2 candidate. Not needed for first LCD + sensor bring-up. |
| SIM800L EVB v2.0 | [sim800l-evb-v20.jpg](hardware/photos/sim800l-evb-v20.jpg) | Excluded from v1 because GSM adds power, timing, memory, and serial complexity. |

## Photo File Map

| Original file | Repository path |
|---|---|
| `20260623_183750.jpg` | `docs/hardware/photos/lcd-12864b-v23-back.jpg` |
| `20260623_183759.jpg` | `docs/hardware/photos/arduino-uno-compatible-back.jpg` |
| `20260623_183809.jpg` | `docs/hardware/photos/openlog-module.jpg` |
| `20260623_183820.jpg` | `docs/hardware/photos/sim800l-evb-v20.jpg` |
| `20260623_183844.jpg` | `docs/hardware/photos/mq-sensors-front.jpg` |
| `20260623_183911.jpg` | `docs/hardware/photos/mq-sensors-labels.jpg` |
| `20260623_184314.jpg` | `docs/hardware/photos/nrf24l01-modules.jpg` |
| `20260623_213451.jpg` | `docs/hardware/photos/aht10-module.jpg` |
