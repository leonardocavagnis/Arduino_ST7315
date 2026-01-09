/*
  Copyright (c) 2026 Arduino SA
  This Source Code Form is subject to the terms of the Mozilla
  Public License (MPL), v 2.0. You can obtain a copy of the MPL
  at http://mozilla.org/MPL/2.0/
*/

#include "Arduino_ST7315.h"

// Determine Wire library buffer size
#if defined(I2C_BUFFER_LENGTH)
#define WIRE_MAX min(256, I2C_BUFFER_LENGTH) ///< Particle or similar Wire lib
#elif defined(BUFFER_LENGTH)
#define WIRE_MAX min(256, BUFFER_LENGTH) ///< AVR or similar Wire lib
#elif defined(SERIAL_BUFFER_SIZE)
#define WIRE_MAX                                                               \
  min(255, SERIAL_BUFFER_SIZE - 1) ///< Newer Wire uses RingBuffer
#else
#define WIRE_MAX 32 ///< Use common Arduino core default
#endif

// ST7315 Wire mode control bytes
#define ST7315_MODE_COMMAND        0x00 // DC = 0
#define ST7315_MODE_DATA           0x40 // DC = 1

// ST7315 Commands
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
    // ST7315 organizes the screen into pages (8 vertical pixels per page) 
    // and columns (horizontal pixels). Before sending the framebuffer data, 
    // we need to tell the display which columns and pages we want to update.
    uint8_t cmdList[] = {
        ST7315_COLUMNADDR,          
        0,                          // Column start address
        uint8_t(width() - 1),       // Column end address
        ST7315_PAGEADDR,            
        0,                          // Page start address
        uint8_t((height() / 8) - 1) // Page end address
    };
    commandList(cmdList, sizeof(cmdList));

    // Total number of bytes to transfer
    uint16_t count = width() * (height() / 8);
    uint8_t *ptr = _buffer;

    _wire->beginTransmission(_address);
    _wire->write(ST7315_MODE_DATA);
    uint16_t bytesOut = 1;

    while (count--) {
        if (bytesOut >= WIRE_MAX) { // Wire buffer limit (splitting the transmission if needed)
            _wire->endTransmission();
            _wire->beginTransmission(_address);
            _wire->write(ST7315_MODE_DATA);
            bytesOut = 1;
        }

        _wire->write(*ptr++);
        bytesOut++;
    }

    _wire->endTransmission();
}

void Arduino_ST7315::command(uint8_t c)
{
    _wire->beginTransmission(_address);
    _wire->write(ST7315_MODE_COMMAND);
    _wire->write(c);
    _wire->endTransmission();
}

void Arduino_ST7315::commandList(const uint8_t *cmds, uint8_t n)
{
    _wire->beginTransmission(_address);
    _wire->write(ST7315_MODE_COMMAND);
    uint16_t bytesOut = 1;

    while (n--) {
        if (bytesOut >= WIRE_MAX) { // Wire buffer limit (splitting the transmission if needed)
            _wire->endTransmission();
            _wire->beginTransmission(_address);
            _wire->write(ST7315_MODE_COMMAND);
            bytesOut = 1;
        }

        _wire->write(*cmds++);
        bytesOut++;
    }

    _wire->endTransmission();
}