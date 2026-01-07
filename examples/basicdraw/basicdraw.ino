/*
  BasicDrawingSSD1306
*/

#include <Wire.h>
#include "Arduino_SSD1306.h"

Arduino_SSD1306 Display(128, 64, &Wire1);

void setup() {

  Wire.begin();

  if (!Display.begin()) {
      while (1) {
        // error loop
      }
  }

  Display.beginDraw();
  Display.background(0);     // schermo nero
  Display.stroke(255);       // colore bianco
  Display.textFont(Font_5x7);

  Display.text("Hello!", 10, 10);
  Display.circle(64, 40, 15);

  Display.endDraw(); // aggiorna il display
}

void loop() {
}