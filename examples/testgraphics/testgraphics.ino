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

  testdrawline();      // Draw many lines

  
}

void loop() {
}

void testdrawline() {
  int16_t i;

  // Clear display buffer
  Display.beginDraw();
  Display.clear();
  Display.endDraw();

  for(i=0; i<Display.width(); i+=4) {
    Display.beginDraw();
    Display.stroke(255, 255, 255); // White color
    Display.line(0, 0, i, Display.height()-1);
    Display.endDraw();
    delay(1);
  }

  for(i=0; i<Display.height(); i+=4) {
    Display.beginDraw();
    Display.stroke(255, 255, 255); // White color
    Display.line(0, 0, Display.width()-1, i);
    Display.endDraw();
    delay(1);
  }
  delay(250);

  Display.beginDraw();
  Display.clear();
  Display.endDraw();

  for(i=0; i<Display.width(); i+=4) {
    Display.beginDraw();
    Display.stroke(255, 255, 255); // White color
    Display.line(0, Display.height()-1, i, 0);
    Display.endDraw();
    delay(1);
  }

  for(i=Display.height()-1; i>=0; i-=4) {
    Display.beginDraw();
    Display.stroke(255, 255, 255); // White color
    Display.line(0, Display.height()-1, Display.width()-1, i);
    Display.endDraw();
    delay(1);
  }
  delay(250);
  
  Display.beginDraw();
  Display.clear();
  Display.endDraw();

  for(i=Display.width()-1; i>=0; i-=4) {
    Display.beginDraw();
    Display.stroke(255, 255, 255); // White color
    Display.line(Display.width()-1, Display.height()-1, i, 0);
    Display.endDraw();
    delay(1);
  }
  for(i=Display.height()-1; i>=0; i-=4) {
    Display.beginDraw();
    Display.stroke(255, 255, 255); // White color
    Display.line(Display.width()-1, Display.height()-1, 0, i);
    Display.endDraw();
    delay(1);
  }
  delay(250);

  Display.beginDraw();
  Display.clear();
  Display.endDraw();

  for(i=0; i<Display.height(); i+=4) {
    Display.beginDraw();
    Display.stroke(255, 255, 255); // White color
    Display.line(Display.width()-1, 0, 0, i);
    Display.endDraw();
    delay(1);
  }

  for(i=0; i<Display.width(); i+=4) {
    Display.beginDraw();
    Display.stroke(255, 255, 255); // White color
    Display.line(Display.width()-1, 0, i, Display.height()-1);
    Display.endDraw();
    delay(1);
  }

  delay(2000); // Pause for 2 seconds
}