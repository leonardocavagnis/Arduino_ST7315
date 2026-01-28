/*
  Copyright (c) 2026 Arduino SA
  This Source Code Form is subject to the terms of the Mozilla
  Public License (MPL), v 2.0. You can obtain a copy of the MPL
  at http://mozilla.org/MPL/2.0/
*/

#include "Arduino_ST7315_Driver.h"

// Determine Wire library max buffer size
#if defined(I2C_BUFFER_LENGTH)      // e.g, Renesas Wire Lib
    #define WIRE_MAX_BUF_LEN min(255, I2C_BUFFER_LENGTH)
#elif defined(BUFFER_LENGTH)        // e.g., AVR Wire Lib
    #define WIRE_MAX_BUF_LEN min(255, BUFFER_LENGTH)
#elif defined(SERIAL_BUFFER_SIZE)   // e.g., SAMD Wire Lib uses RingBuffer
    #define WIRE_MAX_BUF_LEN min(255, SERIAL_BUFFER_SIZE - 1)
#else
    #define WIRE_MAX_BUF_LEN 32     // If not defined, assume 32 bytes (conservative default)
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
#define ST7315_SET_CONTRAST        0x81
#define ST7315_SET_COM_PINS        0xDA
#define SSD1306_DEACTIVATE_SCROLL  0x2E
#define SSD1306_DISPLAYALLON_RESUME 0xA4

Arduino_ST7315_Driver::Arduino_ST7315_Driver(int width, int height, TwoWire *wire, uint8_t address)
    : _width(width)
    , _height(height)
    , _wire(wire)
    , _address(address)
    , _buffer(nullptr)
{
}

Arduino_ST7315_Driver::~Arduino_ST7315_Driver()
{
    if (_buffer) {
        free(_buffer);
    }

    end();
}

int Arduino_ST7315_Driver::begin()
{
    size_t bufferSize = (_width * _height) / 8;

    _buffer = (uint8_t *)malloc(bufferSize);
    if (!_buffer) {
        return 0;
    }
    clear();

    _wire->begin();

    // Init sequence
    const uint8_t initCmds[] = {
        ST7315_DISPLAYOFF,                  // 0xAE: Display OFF during setup
        ST7315_SETDISPLAYCLOCKDIV, 0x80,    // 0xD5 0x80: Set display clock divide ratio / oscillator frequency
        ST7315_SETMULTIPLEX, (uint8_t)(_height-1),  // 0xA8 0x3F: Multiplex ratio = (Display height - 1)
        ST7315_SETDISPLAYOFFSET, 0x00,      // 0xD3 0x00: Display offset = 0 (no vertical shift)
        ST7315_SETSTARTLINE | 0x00,         // 0x40: Set display start line at 0
        ST7315_CHARGEPUMP, 0x14,            // 0x8D 0x14: Enable charge pump (internal DC-DC 0x14 / external 0x10)
        ST7315_MEMORYMODE, 0x00,            // 0x20 0x00: Set memory addressing mode to page addressing
        ST7315_SEGREMAP,                    // 0xA1: Segment remap (flip horizontal direction)
        ST7315_COMSCANDEC,                  // 0xC8: COM output scan direction = remapped (flip vertical direction)
        ST7315_SET_COM_PINS, 0x12,          // 0xDA 0x12: COM pins hardware configuration (FIXME: only for 128x64)
        ST7315_SET_CONTRAST, 0x7F,          // 0x81 0x7F: Set contrast (FIXME: may need adjustment according to voltage supply and display size)
        ST7315_SETPRECHARGE, 0xF1,          // 0xD9 0xF1: Set pre-charge period for stable pixel voltage (FIXME: may need adjustment according to voltage supply)
        ST7315_SETVCOMDETECT, 0x20,         // 0xDB 0x20: Set VCOMH deselect level (for contrast stabilization)
        SSD1306_DISPLAYALLON_RESUME,        // 0xA4: Resume to RAM content display
        ST7315_NORMALDISPLAY,               // 0xA6: Set normal display mode (not inverted)
        SSD1306_DEACTIVATE_SCROLL           // 0x2E: Deactivate scroll if previously enabled
    };
    commandList(initCmds, sizeof(initCmds));
    // Turn display ON as the last step to avoid glitches
    command(ST7315_DISPLAYON);              // 0xAF: Display ON (after all setup commands)

    update(); // Initial display update to clear screen

    return 1;
}

void Arduino_ST7315_Driver::end()
{
    command(ST7315_DISPLAYOFF);
}

void Arduino_ST7315_Driver::set(int x, int y, bool on)
{
    if (!_buffer) {
        return;
    }
    if (x < 0 || y < 0 || x >= _width || y >= _height) {
        return;
    }

    uint16_t index = x + (y / 8) * _width;
    uint8_t  bit  = 1 << (y & 7);

    if (on) {
        _buffer[index] |= bit;
    } else {
        _buffer[index] &= ~bit;
    }
}

void Arduino_ST7315_Driver::clear()
{
    if (_buffer) {
        memset(_buffer, 0, (_width * _height) / 8);
    }
}

void Arduino_ST7315_Driver::update()
{
    // ST7315 organizes the screen into pages (8 vertical pixels per page) 
    // and columns (horizontal pixels). Before sending the framebuffer data, 
    // we need to tell the display which columns and pages we want to update.
    uint8_t cmdList[] = {
        ST7315_COLUMNADDR,          
        0,                          // Column start address
        uint8_t(_width - 1),        // Column end address
        ST7315_PAGEADDR,            
        0,                          // Page start address
        uint8_t((_height / 8) - 1)  // Page end address
    };
    commandList(cmdList, sizeof(cmdList));

    const size_t bufferSize = _width * (_height / 8);
    const uint8_t *ptr      = _buffer;
    size_t remaining        = bufferSize;

    const size_t maxPayload = WIRE_MAX_BUF_LEN - 1; // Reserve 1 byte for mode command byte

    while (remaining > 0) {
        size_t chunkSize = min(remaining, maxPayload);

        _wire->beginTransmission(_address);
        _wire->write(ST7315_MODE_DATA);
        _wire->write(ptr, chunkSize);
        _wire->endTransmission();

        ptr       += chunkSize;
        remaining -= chunkSize;
    }
}

void Arduino_ST7315_Driver::command(uint8_t c)
{
    _wire->beginTransmission(_address);
    _wire->write(ST7315_MODE_COMMAND);
    _wire->write(c);
    _wire->endTransmission();
}

void Arduino_ST7315_Driver::commandList(const uint8_t *cmds, uint8_t n)
{
    const size_t maxPayload = WIRE_MAX_BUF_LEN - 1; // Reserve 1 byte for command byte

    while (n > 0) {
        size_t chunkSize = min<size_t>(n, maxPayload);

        _wire->beginTransmission(_address);
        _wire->write(ST7315_MODE_COMMAND);
        _wire->write(cmds, chunkSize);
        _wire->endTransmission();

        cmds += chunkSize;
        n    -= chunkSize;
    }
}