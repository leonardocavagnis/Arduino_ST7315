/*
  Copyright (c) 2026 Arduino SA
  This Source Code Form is subject to the terms of the Mozilla
  Public License (MPL), v 2.0. You can obtain a copy of the MPL
  at http://mozilla.org/MPL/2.0/
*/

#ifndef _ARDUINO_ST7315_DRIVER_H
#define _ARDUINO_ST7315_DRIVER_H

#include <Wire.h>

// @class Arduino_ST7315_Driver
// @brief Low-level driver for the ST7315 OLED display.
// This class is intended to be used as a low-level driver for the ST7315 OLED display, 
// you can use this library in your graphics library
// or directly to control the display without any graphics abstraction.

class Arduino_ST7315_Driver
{
public:
    Arduino_ST7315_Driver(
        int width = 128,
        int height = 64,
        TwoWire *wire = &Wire1,
        uint8_t address = 0x3D
    );

    ~Arduino_ST7315_Driver();

    int begin();
    void end();

    void set(int x, int y, bool on);
    void clear();
    void update();

private:
    void command(uint8_t c);
    void commandList(const uint8_t *cmds, uint8_t n);

private:
    int         _width;
    int         _height;
    TwoWire     *_wire;
    uint8_t     _address;
    uint8_t     *_buffer;
};

#endif // _ARDUINO_ST7315_DRIVER_H