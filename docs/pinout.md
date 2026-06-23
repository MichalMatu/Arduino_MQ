# Pinout v1

## LCD 12864B-V2.3 to Arduino Uno

Use ST7920 serial mode. LCD `PSB` must be tied to `GND`.

The U8g2 constructor order is `clock`, `data`, `cs`, `reset`.

| LCD pin | LCD label | Arduino Uno | Notes |
|---:|---|---|---|
| 1 | GND / VSS | GND | Common ground. |
| 2 | VCC / VDD | 5V | Logic supply. |
| 3 | V0 / VO | Contrast circuit | Use onboard VR1 first; do not tie directly to 5V. |
| 4 | RS | D10 | U8g2 `cs`. |
| 5 | R/W | D11 | U8g2 `data`, SID in serial mode. |
| 6 | E | D13 | U8g2 `clock`, SCLK in serial mode. |
| 7-14 | DB0-DB7 | Not connected | Unused in serial mode. |
| 15 | PSB | GND | Selects serial mode. |
| 16 | NC | Not connected | Leave open. |
| 17 | RST | D8 | Reset controlled by firmware. |
| 18 | VOUT | Contrast only | Do not use as supply. |
| 19 | BLA | 5V | Backlight anode; use 100 ohm in series if unsure. |
| 20 | BLK | GND | Backlight cathode. |

## MQ Modules to Arduino Uno

Use only analog output `AO`. Leave `DO` disconnected.

| Sensor | Module pin | Arduino Uno |
|---|---|---|
| MQ-2 | AO | A0 |
| MQ-7 | AO | A1 |
| MQ-9 | AO | A2 |
| All MQ | VCC | 5V |
| All MQ | GND | GND |
| All MQ | DO | Not connected |

Read the silkscreen labels on each module before wiring. Do not trust a left-to-right pin order from a photo.

## AHT10 to Arduino Uno

Use I2C. On Arduino Uno, I2C is on analog pins A4/A5.

| AHT10 pin | Arduino Uno |
|---|---|
| VIN | 5V |
| GND | GND |
| SCL | A5 |
| SDA | A4 |

The AHT10 module shown in the project photos has pins labeled `VIN`, `GND`, `SCL`, `SDA`. Do not swap `SCL` and `SDA`.

## Candidate LED Matrix Pins

Do not wire the 8x8 LED matrix until its driver is identified. If it is a `MAX7219` module, reserve these pins to avoid conflicts with LCD, MQ, and AHT10:

| MAX7219 matrix pin | Arduino Uno candidate |
|---|---|
| VCC | 5V |
| GND | GND |
| DIN | D4 |
| CS / LOAD | D5 |
| CLK | D6 |

If the matrix uses `HT16K33`, it should go on I2C instead: `SDA -> A4`, `SCL -> A5`, sharing the bus with AHT10. A bare 8x8 matrix needs a driver and should not be connected directly to many Arduino pins for this project.
