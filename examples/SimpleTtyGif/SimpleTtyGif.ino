/*
  SerialTTYGIF

For ESP32-C3 boards:
Board: ESP32C3 Dev Module
1. Tools > USB CDC On Boot.
2. Change this setting from Disabled to Enabled.  
  
*/

#include <PipeGIF.h>

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
GIF are defined from upper left which perfectly matches
a TTY terminal behavior. Arduino IDE can't write colors
so here is just drawn the 3 color indexes the image have.
##########//////////
##########//////////
##########//////////
######--------//////
######--------//////
//////--------######
//////--------######
//////////##########
//////////##########
//////////##########

A lot of fancy GIF things does not work in this TTY mode:
* Colors
* Animated GIF
* Image placed with offset x,y
* Transparant color
*/
class ttyTerminal : public VirtualDisplay {
public:
  // putPixel() must be overridden.
  // This is the 'callback' from the GIF decoder.
  // It is required to call incrementX() to advance x and y.
  void putPixel(uint8_t _colorIndex) {
    char *symbol = "-#/"; // Different symbols to substitute width index
    int ch = symbol[_colorIndex % sizeof(symbol)];
    Serial.write(ch); // Write two chars to keep aspect ratio nearly the same
    Serial.write(ch);
    if (0 == incrementX()) // When X increment to width, it become 0 again
      Serial.println(); // Goto next line and return to left
  }
};

// Declare variables outside so it is not use stack memory
ttyTerminal* tty;
PipeGIF* gif;

void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  Serial.println("TTY serial ASCII art GIF.");

  // Initialize TTY and apply it to PipeGIF
  tty = new ttyTerminal();
  gif = new PipeGIF(tty);
}

void loop() {
  // Calculate execution time
  unsigned long currentMillis = micros();
  // As we know the size of the image we can use a for-loop
  for (int i = 0; i < sizeof(image); i++) {
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
        Serial.print("GIF decode error.");
        while (true) {}
    }
  }
  Serial.print(micros() - currentMillis);
  Serial.println(" µs execution time");
  while (true) {}
}
