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
#define ST7315_SET_CONTRAST        0x81
#define ST7315_SET_COM_PINS        0xDA
#define SSD1306_DEACTIVATE_SCROLL  0x2E
#define SSD1306_DISPLAYALLON_RESUME 0xA4

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
    clearDisplay();

    _wire->begin();

    // Init sequence
    const uint8_t initCmds[] = {
        ST7315_DISPLAYOFF,                  // 0xAE: Display OFF during setup
        ST7315_SETDISPLAYCLOCKDIV, 0x80,    // 0xD5 0x80: Set display clock divide ratio / oscillator frequency
        ST7315_SETMULTIPLEX, (height()-1),  // 0xA8 0x3F: Multiplex ratio = (Display height - 1)
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

    updateDisplay(); // Initial display update to clear screen

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

void Arduino_ST7315::clearDisplay()
{
    if (_buffer) {
        memset(_buffer, 0, (width() * height()) / 8);
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