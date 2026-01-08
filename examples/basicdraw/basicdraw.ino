/*
  BasicDrawingSSD1306
*/

#include <Wire.h>
#include "Arduino_SSD1306.h"

#define SCREEN_WIDTH    128 // OLED display width, in pixels
#define SCREEN_HEIGHT   64 // OLED display height, in pixels
#define SCREEN_ADDRESS  0x3C // I2C address for the SSD1306

Arduino_SSD1306 Display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, SCREEN_ADDRESS);

void setup() {
  if (!Display.begin()) {
      while (1) ;
  }

  Display.beginDraw();
  Display.background(0);
  Display.stroke(255);
  Display.textFont(Font_5x7);

  Display.text("Hello!", 10, 10);
  Display.circle(64, 40, 15);

  Display.endDraw();
}

void loop() {
}