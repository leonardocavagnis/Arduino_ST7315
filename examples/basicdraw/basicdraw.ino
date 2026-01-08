/**
 * This sketch initializes an SSD1306 OLED display via I2C and renders
 * simple graphics including text and shapes (circle) on the screen.
 */

#include <Wire.h>
#include "Arduino_SSD1306.h"

#define SCREEN_WIDTH    128 // OLED display width, in pixels
#define SCREEN_HEIGHT   64 // OLED display height, in pixels
#define SCREEN_ADDRESS  0x3C // I2C address for the SSD1306

Arduino_SSD1306 Display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, SCREEN_ADDRESS);

void setup() {
    Serial.begin(115200);
    
    if (!Display.begin()) {
        Serial.println("SSD1306 Display allocation failed");
        while (1) ;
    }

    Display.beginDraw();
    Display.background(0, 0, 0);
    Display.stroke(255, 255, 255);
    Display.textFont(Font_5x7);

    Display.text("Hello Arduino!", 10, 10);
    Display.circle(64, 40, 15);

    Display.endDraw();
}

void loop() {
    // Nothing to do here
}