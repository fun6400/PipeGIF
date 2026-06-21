/*
 * OledAdafruit72x40
 * by Hans Schou <hans@schou.dk> © 2026
 * SPDX-License-Identifier: MIT
 *
 * Test with ESP32-C3 OLED 0.42" 72x40 SSD1306 development board.
 * Sketch uses 336722 bytes (25%) of program storage space. Maximum is 1310720 bytes.
 */

#include "Adafruit_SSD1306_72x40.h"
#include <PipeGIF.h>

#define SDA_PIN 5            // I²C Data
#define SCL_PIN 6            // I²C Clock

Adafruit_SSD1306_72x40 display(SDA_PIN, SCL_PIN);

/*
 In color (red,blue,white) the image would look like:
 RRRRRbbbbb
 RRRwwwwbbb
 bbbwwwwRRR
 bbbbbRRRRR
 The original image can be found here:
 https://giflib.sourceforge.net/whatsinagif/lzw_image_data.html
*/
const uint8_t image[] PROGMEM = {
  0x47, 0x49, 0x46, 0x38, 0x39, 0x61,                          // GIF89a
  0x0a, 0x00, 0x0a, 0x00, 0x91, 0x00, 0x00,                    // Logical screen descriptor 10x10, color count 4=2^(1+1)
  0xff, 0xff, 0xff,                                            // White Color table
  0xff, 0x00, 0x00,                                            // Red
  0x00, 0x00, 0xff,                                            // Blue
  0x00, 0x00, 0x00,                                            // Color not used, filler
  0x2c, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x0a, 0x00, 0x00,  // Image Descriptor 10x10
  0x02,                                                        // LZW code size = 2
  0x16,                                                        // blockSize = 22 bytes in this block
  0x8c, 0x2d, 0x99, 0x87, 0x2a, 0x1c, 0xdc, 0x33,              // LZW 0..7
  0xa0, 0x02, 0x75, 0xec, 0x95, 0xfa, 0xa8, 0xde,              // LZW 8..15
  0x60, 0x8c, 0x04, 0x91, 0x4c, 0x01,                          // LZW 16..22
  0x00,                                                        // blockSize of next block, 0 == end-of-LZW
  0x3b,                                                        // GIF file terminator
};

/*
The GIF image would look like below with solid
color in upper left and lower right. Vertical 
lines in the middle. Horizontal lines in the
coners.
||||||||||----------
||||||||||----------
||||||||||----------
||||||########------
||||||########------
------########||||||
------########||||||
----------||||||||||
----------||||||||||
----------||||||||||
*/
class adafruitDisplay : public VirtualDisplay {
public:
  // putPixel() must be overridden.
  // This is the 'callback' from the GIF decoder.
  // It is required to call incrementX() to advance x and y.
  void putPixel(uint8_t _colorIndex) {
    switch (_colorIndex) {
      case 0:  // Solid white
        display.drawPixel(x * 2, y * 2, SSD1306_WHITE);
        display.drawPixel(x * 2 + 1, y * 2, SSD1306_WHITE);
        display.drawPixel(x * 2, y * 2 + 1, SSD1306_WHITE);
        display.drawPixel(x * 2 + 1, y * 2 + 1, SSD1306_WHITE);
        break;
      case 1:  // Vertical lines
        display.drawPixel(x * 2, y * 2, SSD1306_BLACK);
        display.drawPixel(x * 2 + 1, y * 2, SSD1306_WHITE);
        display.drawPixel(x * 2, y * 2 + 1, SSD1306_BLACK);
        display.drawPixel(x * 2 + 1, y * 2 + 1, SSD1306_WHITE);
        break;
      case 2:  // Horizontal lines
        display.drawPixel(x * 2, y * 2, SSD1306_WHITE);
        display.drawPixel(x * 2 + 1, y * 2, SSD1306_WHITE);
        display.drawPixel(x * 2, y * 2 + 1, SSD1306_BLACK);
        display.drawPixel(x * 2 + 1, y * 2 + 1, SSD1306_BLACK);
        break;
    }
    if (0 == incrementX())  // When 'x' increment to width, it become 0 again
    {}
  }
};

void printDuration(unsigned long duration) {
  char txtStr[35];
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(24, 0);
  display.print("PipeGIF");
  display.setCursor(24, 10);
  display.print("Adafruit");
  snprintf(txtStr, sizeof(txtStr), "%lu usec", duration);
  display.setCursor(0, 22);
  display.print(txtStr);
  display.setCursor(0, 32);
  display.print("72x40 px");
}

// Declare variables outside so it does not use stack memory
adafruitDisplay* dsp;
PipeGIF* gif;

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  if (!display.begin()) {
    // Init failed
    for (;;);
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(1000);
  display.clearDisplay();

  // Initialize display and apply it to PipeGIF
  dsp = new adafruitDisplay();
  gif = new PipeGIF(dsp);

  // Calculate execution time
  unsigned long currentMillis = micros();
  // As we know the size of the image we can use a for-loop
  for (uint8_t i = 0; i < sizeof(image); i++) {
    // Decode one byte at the time.
    // If required it will call our TTY->putPixel()
    switch (gif->decode(image[i])) {
      case GIF_OK:
        break;
      case GIF_DELAY100:
        // Animation not used in this example
        break;
      case GIF_TRAILER:
        // End-of-file
        break;
      default:
        printDuration(0);
    }
  }
  printDuration(micros() - currentMillis);
  display.display();
}

void loop() {
}
