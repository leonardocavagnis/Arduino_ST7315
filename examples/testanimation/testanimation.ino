/*
  Test Animation for ST7315 OLED Display

  This example demonstrates a simple animation of a bouncing ball
  on an ST7315 OLED display using the Arduino_ST7315 library.
  
  by Leonardo Cavagnis
*/

#include "Arduino_ST7315.h"

#define SCREEN_WIDTH    128   // OLED display width, in pixels
#define SCREEN_HEIGHT   64    // OLED display height, in pixels
#define SCREEN_ADDRESS  0x3C  // I2C address for the ST7315

Arduino_ST7315 Display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, SCREEN_ADDRESS);

const int radius = 4;
const int frameDelay = 20;

int vx = 2;
int vy = 2; 

int x = SCREEN_WIDTH  / 2;
int y = SCREEN_HEIGHT / 2;

void setup() {
    Serial.begin(115200);
    
    if (!Display.begin()) {
        Serial.println("ST7315 Display allocation failed");
        while (1) ;
    }
}

void loop() {
    x += vx;
    y += vy;

    if (x - radius <= 0 || x + radius >= Display.width()) {
        vx = -vx;
    }
    if (y - radius <= 0 || y + radius >= Display.height()) {
        vy = -vy;
    }

    Display.beginDraw();
    Display.clear();

    Display.noStroke();
    Display.fill(255, 255, 255); 
    Display.circle(x, y, radius * 2);

    Display.endDraw();

    delay(frameDelay);
}