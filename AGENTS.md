# Project Instructions

- Target firmware v1 is only for Arduino Uno-compatible ATmega328P, LCD 12864B-V2.3 in ST7920 serial mode, and MQ-2/MQ-7/MQ-9 analog modules.
- Do not add SIM800L, nRF24L01+, OpenLog, ppm calculations, network code, or persistent logging to v1.
- Keep Arduino Uno limits in mind: avoid `String`, heap allocation, full-frame LCD buffers, and large global tables in SRAM.
- Use U8g2 page-buffer constructors and fixed-size stack buffers.
- Display and log RAW ADC plus millivolts only. MQ-7 and MQ-9 heater cycles are not controlled by these common 4-pin modules.

