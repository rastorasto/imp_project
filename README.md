# IMP — Scrolling RGB LED Matrix (ESP32)

A scrolling-message LED display built on a Wemos D1 R32: a 16×8 RGB matrix
driven through a 74HCT154 column decoder and a TLC5947 24-channel PWM
controller over SPI, time-multiplexed at 50 Hz.

## What it does

- Scrolls arbitrary text across the matrix
- Four buttons: message / speed / color / brightness
- Own 5×7 bitmap font rendered on-device
- `documentation.md` — full hardware + build docs; demo `.mp4` included

## Stack

A single Arduino sketch, no framework. Flash via Arduino IDE.

Semestral project for *Mikroprocesorové a vestavné systémy (IMP)* at FIT VUT
Brno.
