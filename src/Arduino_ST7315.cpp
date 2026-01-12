/*
  Copyright (c) 2026 Arduino SA
  This Source Code Form is subject to the terms of the Mozilla
  Public License (MPL), v 2.0. You can obtain a copy of the MPL
  at http://mozilla.org/MPL/2.0/
*/

#include "Arduino_ST7315.h"

Arduino_ST7315::Arduino_ST7315(int width, int height, TwoWire *wire, uint8_t address)
    : ArduinoGraphics(width, height),
      _driver(width, height, wire, address)
{
}

int Arduino_ST7315::begin()
{
    return _driver.begin();
}

void Arduino_ST7315::end()
{
    _driver.end();
}

void Arduino_ST7315::endDraw()
{
    _driver.update();
}

void Arduino_ST7315::set(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    bool on = (r | g | b) > 127; // Simple thresholding for monochrome
    _driver.set(x, y, on);
}
