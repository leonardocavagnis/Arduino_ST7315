# 🖥️ Arduino SSD1306 Library

This library allows you to control SSD1306-based OLED displays using an Arduino board.

The SSD1306 is a single-chip CMOS OLED/PLED driver and controller designed for dot-matrix graphic displays.
It communicates over the I²C (Wire) bus and integrates display RAM, an internal oscillator, and voltage generation circuitry, making it ideal for embedded user interfaces, wearable devices, and compact information displays.

## 🖌️ Graphics API
This library is built on top of the Arduino Graphics library and uses its APIs to draw content on the screen.
All drawing operations (text, lines, rectangles, images, bitmaps, scrolling, etc.) are performed using the Arduino Graphics interface, providing a consistent and hardware-agnostic way to render graphics.

🔗 Arduino Graphics library:
https://github.com/arduino-libraries/ArduinoGraphics
