/*
 * Copyright 2025 Arduino SA
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef _ARDUINO_SSD1306_H
#define _ARDUINO_SSD1306_H

#include <Arduino.h>
#include <Wire.h>
#include "ArduinoGraphics.h"

class Arduino_SSD1306 : public ArduinoGraphics
{
public:
    Arduino_SSD1306(
        int width = 128,
        int height = 64,
        TwoWire *wire = &Wire,
        uint8_t address = 0x3C
    );

    ~Arduino_SSD1306();

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