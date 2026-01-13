# 🖥️ Arduino ST7315 Library

This library allows you to control ST7315-based OLED displays using an Arduino board.

The ST7315 is a single-chip CMOS OLED/PLED driver and controller designed for dot-matrix graphic displays.
It communicates over the `Wire` (I²C) interface and integrates display RAM, an internal oscillator, and voltage generation circuitry, making it ideal for embedded user interfaces, wearable devices, and compact information displays.

## 🖌️ Graphics API
The `Arduino_ST7315` library is built on top of the Arduino Graphics library and uses its APIs to draw content on the screen.

🔗 Arduino Graphics library:
https://github.com/arduino-libraries/ArduinoGraphics

## ⚙️ Low-level Driver API
This library also provides a low-level driver class (`Arduino_ST7315_Driver`) that can be used independently of the graphics abstraction layer.

You can use this driver:
- As a backend for your own graphics library
- Directly to control the display hardware without graphics abstraction
- To implement custom display control logic