/*
  Copyright (c) 2026 Arduino SA
  This Source Code Form is subject to the terms of the Mozilla
  Public License (MPL), v 2.0. You can obtain a copy of the MPL
  at http://mozilla.org/MPL/2.0/
*/

#ifndef _ARDUINO_ST7315_H
#define _ARDUINO_ST7315_H

#include <Arduino.h>
#include <Wire.h>
#include "ArduinoGraphics.h"

class Arduino_ST7315 : public ArduinoGraphics
{
public:
    Arduino_ST7315(
        int width = 128,
        int height = 64,
        TwoWire *wire = &Wire1,
        uint8_t address = 0x3C
    );

    ~Arduino_ST7315();

    int begin();
    void end();

    virtual void beginDraw();
    virtual void endDraw();
    virtual void set(int x, int y, uint8_t r, uint8_t g, uint8_t b);

private:
    void command(uint8_t c);
    void commandList(const uint8_t *cmds, uint8_t n);
    void updateDisplay();

private:
    TwoWire   *_wire;
    uint8_t    _address;
    uint8_t   *_buffer;
};

#endif