#include <Adafruit_SSD1306_72x40.h>

/*
 * OledU8g2
 * by Hans Schou <hans@schou.dk> © 2026
 * SPDX-License-Identifier: MIT
 *
 * Test with ESP32-C3 OLED 1.3" 128x64 SSD1306 development board,
 * and ESP32-C3 OLED 0.42" 72x40 SSD1306.
 * Sketch uses 336722 bytes (25%) of program storage space. Maximum is 1310720 bytes.
 * 12040 µs execution time.
 */

#include <PipeGIF.h>
#include <U8g2lib.h>

#define SDA_PIN 5  // I²C Data
#define SCL_PIN 6  // I²C Clock

/* Uncomment one of the boards to test with. */
U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);  // EastRising 0.42" OLED
//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE); // 1.3" OLED

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
color in the middle. Vertical lines in the left
top corner and lower right corner. Horizontal
lines in the other coners.
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
class u8g2Display : public VirtualDisplay {
public:
  // putPixel() must be overridden.
  // This is the 'callback' from the GIF decoder.
  // It is required to call incrementX() to advance x and y.
  void putPixel(uint8_t _colorIndex) {
    switch (_colorIndex) {
      case 0:  // Solid white
        u8g2.setDrawColor(1);
        u8g2.drawPixel(x * 2, y * 2);
        u8g2.drawPixel(x * 2 + 1, y * 2);
        u8g2.drawPixel(x * 2, y * 2 + 1);
        u8g2.drawPixel(x * 2 + 1, y * 2 + 1);
        break;
      case 1:  // Vertical lines
        u8g2.setDrawColor(0);
        u8g2.drawPixel(x * 2, y * 2);
        u8g2.setDrawColor(1);
        u8g2.drawPixel(x * 2 + 1, y * 2);
        u8g2.setDrawColor(0);
        u8g2.drawPixel(x * 2, y * 2 + 1);
        u8g2.setDrawColor(1);
        u8g2.drawPixel(x * 2 + 1, y * 2 + 1);
        break;
      case 2:  // Horizontal lines
        u8g2.setDrawColor(1);
        u8g2.drawPixel(x * 2, y * 2);
        u8g2.drawPixel(x * 2 + 1, y * 2);
        u8g2.setDrawColor(0);
        u8g2.drawPixel(x * 2, y * 2 + 1);
        u8g2.drawPixel(x * 2 + 1, y * 2 + 1);
        break;
    }
    if (0 == incrementX())  // When 'x' increment to width, it become 0 again
    {}                      // Do something when going to next line
  }
};

void printDuration(unsigned long duration) {
  char txtStr[35];
  u8g2.drawStr(24, 0, "PipeGIF");  
  u8g2.drawStr(24, 10, "U8g2 lib");  
  snprintf(txtStr, sizeof(txtStr), "%lu usec", duration);
  u8g2.drawStr(0, 22, txtStr);
  snprintf(txtStr, sizeof(txtStr), "%ux%u px", u8g2.getDisplayWidth(), u8g2.getDisplayHeight());
  u8g2.drawStr(0, 32, txtStr);
}

// Declare variables outside so it does not use stack memory
u8g2Display* dsp;
PipeGIF* gif;

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
  u8g2.clearBuffer();

  // Initialize display and apply it to PipeGIF
  dsp = new u8g2Display();
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
      //default:
        // GIF decode error
    }
  }
  u8g2.sendBuffer();
  printDuration(micros() - currentMillis);
  u8g2.sendBuffer();
  while (1) {}
}

void loop() {
}
