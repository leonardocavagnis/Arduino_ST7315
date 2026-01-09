/*
  Copyright (c) 2026 Arduino SA
  This Source Code Form is subject to the terms of the Mozilla
  Public License (MPL), v 2.0. You can obtain a copy of the MPL
  at http://mozilla.org/MPL/2.0/
*/

#include "Arduino_ST7315.h"

// Core ST7315 commands
#define ST7315_DISPLAYOFF          0xAE
#define ST7315_DISPLAYON           0xAF
#define ST7315_COLUMNADDR          0x21
#define ST7315_PAGEADDR            0x22
#define ST7315_SETDISPLAYCLOCKDIV  0xD5
#define ST7315_SETMULTIPLEX        0xA8
#define ST7315_SETDISPLAYOFFSET    0xD3
#define ST7315_SETSTARTLINE        0x40
#define ST7315_CHARGEPUMP          0x8D
#define ST7315_MEMORYMODE          0x20
#define ST7315_SEGREMAP            0xA1
#define ST7315_COMSCANDEC          0xC8
#define ST7315_SETPRECHARGE        0xD9
#define ST7315_SETVCOMDETECT       0xDB
#define ST7315_DISPLAYALLON_RESUME 0xA4
#define ST7315_NORMALDISPLAY       0xA6

Arduino_ST7315::Arduino_ST7315(int width, int height, TwoWire *wire, uint8_t address)
    : ArduinoGraphics(width, height)
    , _wire(wire)
    , _address(address)
    , _buffer(nullptr)
{
}

Arduino_ST7315::~Arduino_ST7315()
{
    if (_buffer) {
        free(_buffer);
    }
}

int Arduino_ST7315::begin()
{
    size_t bufferSize = (width() * height()) / 8;

    _buffer = (uint8_t *)malloc(bufferSize);
    if (!_buffer) {
        return 0;
    }
    memset(_buffer, 0, bufferSize);

    _wire->begin();

    // Init sequence
    const uint8_t initCmds[] = {
        ST7315_DISPLAYOFF,
        ST7315_SETDISPLAYCLOCKDIV, 0x80,
        ST7315_SETMULTIPLEX, 0x3F,
        ST7315_SETDISPLAYOFFSET, 0x00,
        ST7315_SETSTARTLINE | 0x00,
        ST7315_CHARGEPUMP, 0x14,
        ST7315_MEMORYMODE, 0x00,
        ST7315_SEGREMAP | 0x01,
        ST7315_COMSCANDEC,
        ST7315_SETPRECHARGE, 0xF1,
        ST7315_SETVCOMDETECT, 0x40,
        ST7315_DISPLAYALLON_RESUME,
        ST7315_NORMALDISPLAY
    };

    commandList(initCmds, sizeof(initCmds));
    command(ST7315_DISPLAYON);

    return 1;
}

void Arduino_ST7315::end()
{
    command(ST7315_DISPLAYOFF);
}

void Arduino_ST7315::beginDraw()
{
    // nothing required
}

void Arduino_ST7315::endDraw()
{
    updateDisplay();
}

void Arduino_ST7315::set(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    if (!_buffer) {
        return;
    }
    if (x < 0 || y < 0 || x >= width() || y >= height()) {
        return;
    }

    bool on = (r | g | b) > 127;

    uint16_t index = x + (y / 8) * width();
    uint8_t  bit  = 1 << (y & 7);

    if (on) {
        _buffer[index] |= bit;
    } else {
        _buffer[index] &= ~bit;
    }
}

void Arduino_ST7315::updateDisplay()
{
    command(ST7315_COLUMNADDR);
    command(0);
    command(width()-1);

    command(ST7315_PAGEADDR);
    command(0);
    command((height()/8)-1);

    uint16_t total = (width() * height()) / 8;

    for (uint16_t i = 0; i < total; i += 16) {
        _wire->beginTransmission(_address);
        _wire->write(0x40);

        uint8_t chunk = (total - i >= 16) ? 16 : (total - i);

        for (uint8_t n = 0; n < chunk; n++) {
            _wire->write(_buffer[i + n]);
        }

        _wire->endTransmission();
    }
}

void Arduino_ST7315::command(uint8_t c)
{
    _wire->beginTransmission(_address);
    _wire->write(0x00);
    _wire->write(c);
    _wire->endTransmission();
}

void Arduino_ST7315::commandList(const uint8_t *cmds, uint8_t n)
{
    while (n--) {
        command(*cmds++);
    }
}