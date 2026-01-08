
/**
 * This sketch initializes an SSD1306 OLED display via I2C and demonstrates various 
 * graphics capabilities of the library including drawing lines, rectangles, circles, 
 * text rendering with different styles and sizes, scrolling text effects, and image display.
 */

#include <Wire.h>
#include "Arduino_SSD1306.h"
#include "img_arduinologobw.h"

#define SCREEN_WIDTH    128   // OLED display width, in pixels
#define SCREEN_HEIGHT   64    // OLED display height, in pixels
#define SCREEN_ADDRESS  0x3C  // I2C address for the SSD1306

Arduino_SSD1306 Display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, SCREEN_ADDRESS);

Image img_arduinologobw(ENCODING_RGB16, (uint8_t *) img_arduinologobw_raw, 60, 60);

void setup() {
    Serial.begin(115200);
    
    if (!Display.begin()) {
        Serial.println("SSD1306 Display allocation failed");
        while (1) ;
    }

    testDrawLine();     // Draw many lines

    testDrawRect();     // Draw rectangles

    testFillRect();     // Draw filled rectangles

    testDrawCircle();   // Draw circles

    testFillCircle();   // Draw filled circles 

    testDrawChar();     // Draw characters

    testDrawStyles();   // Demonstrate text styles

    testScrollText();   // Demonstrate scrolling text

    testDrawImage();    // Draw image from memory

    testAnimation();    // Simple animation    
}

void loop() {
    // Nothing to do here
}

void testDrawLine() {
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

  delay(2000);
}

void testDrawRect(void) {
  Display.beginDraw();
  Display.clear();
  Display.endDraw();

  for(int16_t i=0; i<Display.height()/2; i+=2) {
    Display.beginDraw();
    Display.stroke(255, 255, 255); // White color
    Display.noFill();
    Display.rect(i, i, Display.width()-2*i, Display.height()-2*i);
    Display.endDraw();
    delay(1);
  }

  delay(2000);
}

void testFillRect(void) {
    Display.beginDraw();
    Display.clear();
    Display.endDraw();

  for(int16_t i=0; i<Display.height()/2; i+=3) {
    Display.beginDraw();
    Display.noStroke();
    if (i%2 == 0) Display.fill(255, 255, 255); // White color
    else Display.fill(0, 0, 0); // Black color
    Display.rect(i, i, Display.width()-i*2, Display.height()-i*2);
    Display.endDraw();
    delay(1);
  }

  delay(2000);
}

void testDrawCircle(void) {
    Display.beginDraw();
    Display.clear();
    Display.endDraw();

  for(int16_t i=0; i<max(Display.width(),Display.height())/2; i+=2) {
    Display.beginDraw();
    Display.stroke(255, 255, 255); // White color
    Display.noFill();
    Display.circle(Display.width()/2, Display.height()/2, i);
    Display.endDraw();
    delay(1);
  }

  delay(2000);
}

void testFillCircle(void) {
    Display.beginDraw();
    Display.clear();
    Display.endDraw();

  for(int16_t i=max(Display.width(),Display.height())/2; i>0; i-=3) {
    Display.beginDraw();
    Display.noStroke();
    if (i%2 == 0) Display.fill(255, 255, 255); // White color
    else Display.fill(0, 0, 0); // Black color
    Display.circle(Display.width()/2, Display.height()/2, i);
    Display.endDraw();
    delay(1);
  }

  delay(2000);
}

void testDrawChar(void) {
  int maxCols = Display.width()  / 6; // 5px + 1px spacing (Font_5x7)
  int maxRows = Display.height() / 8; // 7px + 1px spacing (Font_5x7)
  
  char str[2] = {0, 0};
  int ascii = 32; // printable chars
  
  Display.beginDraw();
  Display.clear();
  Display.stroke(255, 255, 255);
  Display.textFont(Font_5x7);

  for (int row = 0; row < maxRows; row++) {
    for (int col = 0; col < maxCols; col++) {
        if (ascii >= 128)
            goto end;   // display full or ASCII ended

        str[0] = (char)ascii++;

        int x = col * 6; // 5px + 1px spacing (Font_5x7)
        int y = row * 8; // 7px + 1px spacing (Font_5x7)

        Display.text(str, x, y);
    }
}

  end:
    Display.endDraw();
  
  delay(2000);
}

void testDrawStyles() {
    Display.beginDraw();
    Display.clear();
    Display.textFont(Font_5x7);

    int x = 0;
    int y = 0;

    // Normal "Hello, world!" text
    Display.stroke(255, 255, 255);  // White text color
    Display.background(0, 0, 0);    // Black background
    Display.textSize(1);            // Normal 1:1 scale
    Display.text("Hello, world!", x, y);

    // Move Y to the next line
    y += Display.textFontHeight() * 1;

    // Inverse text (black on white) ---
    Display.stroke(0, 0, 0);        // Text color: black
    Display.background(255, 255, 255); // Background color: white
    char piStr[16];
    sprintf(piStr, "%.2f", 3.14);
    Display.text(piStr, x, y);

    // Move Y to the next line
    y += Display.textFontHeight() * 1;

    // Enlarged text (2X scale)
    Display.stroke(255, 255, 255);  // White text
    Display.background(0, 0, 0);    // Black background
    Display.textSize(2);            // 2X scaling

    // Print hexadecimal value "0xDEADBEEF"
    char hexStr[16];
    sprintf(hexStr, "0x%X", 0xDEADBEEF);
    Display.text(hexStr, x, y);

    Display.endDraw();
    delay(2000);
}

void testScrollText() {
    const char* text = "scroll"; // Text to scroll
    int speed = 50;              // Scroll speed in ms per pixel

    // Prepare the display
    Display.beginDraw();
    Display.clear();
    Display.textFont(Font_5x7);
    Display.textSize(2);         // 2X scale text
    Display.stroke(255, 255, 255); // White text
    Display.background(0, 0, 0);   // Black background
    Display.endDraw();

    delay(100);

    // Scroll RIGHT
    Display.beginText(0, 0, 255, 255, 255); // Top-left corner, white text
    Display.textScrollSpeed(speed);
    Display.print(text);
    Display.endText(SCROLL_RIGHT);
    delay(2000);

    // Scroll LEFT
    Display.beginText(0, 0, 255, 255, 255);
    Display.textScrollSpeed(speed);
    Display.print(text);
    Display.endText(SCROLL_LEFT);
    delay(2000);

    // Scroll DOWN
    Display.beginText(0, 0, 255, 255, 255);
    Display.textScrollSpeed(speed);
    Display.print(text);
    Display.endText(SCROLL_DOWN);
    delay(2000);

    // Scroll UP
    Display.beginText(0, 0, 255, 255, 255);
    Display.textScrollSpeed(speed);
    Display.print(text);
    Display.endText(SCROLL_UP);
}

void testDrawImage() {
    Display.beginDraw();
    Display.background(255, 255, 255); // White background
    Display.clear();
    Display.image(img_arduinologobw, (Display.width() - img_arduinologobw.width())/2, (Display.height() - img_arduinologobw.height())/2);
    Display.endDraw();
    delay(2000);
}

void testAnimation() {
    const int radius = 4;
    const int frameDelay = 20;
    const int frames = 300;

    int x = Display.width()  / 2;
    int y = Display.height() / 2;

    int vx = 2;
    int vy = 2;

    Display.beginDraw();
    Display.background(0, 0, 0);
    Display.clear();
    Display.endDraw();

    for (int i = 0; i < frames; i++) {
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

    delay(2000);
}