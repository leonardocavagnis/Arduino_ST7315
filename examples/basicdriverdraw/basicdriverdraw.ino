/*
  Basic Driver Example for ST7315 OLED Display

  This example demonstrates how to draw a simple square
  at the center of the ST7315 OLED display using the
  low-level Arduino_ST7315_Driver class without any graphics library.

  by Leonardo Cavagnis
*/

#include "Arduino_ST7315_Driver.h"

#define SCREEN_WIDTH    128     // OLED display width, in pixels
#define SCREEN_HEIGHT   64      // OLED display height, in pixels
#define SCREEN_ADDRESS  0x3D    // I2C address for the ST7315

Arduino_ST7315_Driver Display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, SCREEN_ADDRESS);

void setup() {
    Serial.begin(115200);

    if (!Display.begin()) {
        Serial.println("ST7315 Driver initialization failed");
        while (1);
    }

    // Clear internal framebuffer
    Display.clear();

    // Draw filled square pixel by pixel
    int size = 20;
    int startX = (SCREEN_WIDTH - size) / 2;
    int startY = (SCREEN_HEIGHT - size) / 2;

    for (int x = startX; x < startX + size; x++)
        for (int y = startY; y < startY + size; y++)
            Display.set(x, y, true);

    // Send framebuffer to the display
    Display.update();
}

void loop() {
    // Nothing to do here
}