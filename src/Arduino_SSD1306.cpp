#include "Arduino_SSD1306.h"

// Core SSD1306 commands
#define SSD1306_DISPLAYOFF          0xAE
#define SSD1306_DISPLAYON           0xAF
#define SSD1306_COLUMNADDR          0x21
#define SSD1306_PAGEADDR            0x22
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5
#define SSD1306_SETMULTIPLEX        0xA8
#define SSD1306_SETDISPLAYOFFSET    0xD3
#define SSD1306_SETSTARTLINE        0x40
#define SSD1306_CHARGEPUMP          0x8D
#define SSD1306_MEMORYMODE          0x20
#define SSD1306_SEGREMAP            0xA1
#define SSD1306_COMSCANDEC          0xC8
#define SSD1306_SETPRECHARGE        0xD9
#define SSD1306_SETVCOMDETECT       0xDB
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_NORMALDISPLAY       0xA6

Arduino_SSD1306::Arduino_SSD1306(int width, int height, TwoWire *wire, uint8_t address)
    : ArduinoGraphics(width, height)
    , _wire(wire)
    , _address(address)
    , _buffer(nullptr)
{
}

Arduino_SSD1306::~Arduino_SSD1306()
{
    if (_buffer) {
        free(_buffer);
    }
}

int Arduino_SSD1306::begin()
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
        SSD1306_DISPLAYOFF,
        SSD1306_SETDISPLAYCLOCKDIV, 0x80,
        SSD1306_SETMULTIPLEX, 0x3F,
        SSD1306_SETDISPLAYOFFSET, 0x00,
        SSD1306_SETSTARTLINE | 0x00,
        SSD1306_CHARGEPUMP, 0x14,
        SSD1306_MEMORYMODE, 0x00,
        SSD1306_SEGREMAP | 0x01,
        SSD1306_COMSCANDEC,
        SSD1306_SETPRECHARGE, 0xF1,
        SSD1306_SETVCOMDETECT, 0x40,
        SSD1306_DISPLAYALLON_RESUME,
        SSD1306_NORMALDISPLAY
    };

    commandList(initCmds, sizeof(initCmds));
    command(SSD1306_DISPLAYON);

    return 1;
}

void Arduino_SSD1306::end()
{
    command(SSD1306_DISPLAYOFF);
}

void Arduino_SSD1306::beginDraw()
{
    // nothing required
}

void Arduino_SSD1306::endDraw()
{
    updateDisplay();
}

void Arduino_SSD1306::set(int x, int y, uint8_t r, uint8_t g, uint8_t b)
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

void Arduino_SSD1306::updateDisplay()
{
    command(SSD1306_COLUMNADDR);
    command(0);
    command(width()-1);

    command(SSD1306_PAGEADDR);
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

void Arduino_SSD1306::command(uint8_t c)
{
    _wire->beginTransmission(_address);
    _wire->write(0x00);
    _wire->write(c);
    _wire->endTransmission();
}

void Arduino_SSD1306::commandList(const uint8_t *cmds, uint8_t n)
{
    while (n--) {
        command(*cmds++);
    }
}