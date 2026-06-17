# PipeGIF

GIF decoder piping data from one program to another

GIF-Data -> gifdecoder -> lzwdecoder -> display

The GIF decoder is made with a Stream Parser or Finite State Machine (FSM)
thus reading from the data source is not needed.

Features:
* GIF decoding
* Any data source: const, file, SD-card, network
* Animated GIF
* Interlaced image
* GIF87a & GIF89a
* Transparant
* Buffer: only GIF header and color table
* Any display

Requirements:
* About 20 KB RAM for GIF structs and LZW stacks
* [Little Endian](https://en.wikipedia.org/wiki/Endianness)
  byte order MCU or CPU
* Gcc or Clang compiler which can pack struct bits in the right order

## Examples, Arduino

The examples are tested with an ESP32-C3 with an
OLED 0.42" 72x40 display and an OLED 1.3" 128x64 display.

[SimpleTtyGif.ino](examples/SimpleTtyGif/SimpleTtyGif.ino)
is a simple example which just require Serial.print()
to demonstrate the GIF decoder producing ASCII art.
A `const` image are decoded to the serial console.

[OledU8g2.ino](examples/OledU8g2/OledU8g2.ino)
uses the [U8g2](https://github.com/olikraus/u8g2/)
graphics library and requires an OLED black
and white display controlled by a SSD1306 chip.
The example is prepared for either 72x40 or 128x64
pixels display.

[OledAdafruit.ino](examples/OledAdafruit/OledAdafruit.ino)
uses the
[Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library/)
and requires an OLED black and white display controlled
by a SSD1306 chip.
The example is prepared for either 72x40 or 128x64 pixels.

## Examples, Linux

For developing and testing the PipeGIF library Linux was
used as it is faster and more convenient for debugging.
GUI was not used as a TUI terminal is sufficient.

To compile the examples run `make` in the
[linux](linux/) directory.

[inlinestruct.cpp](linux/inlinestruct.cpp)
is a simple example where the data is an inline
`const struct` showing the image with ANSI colors
on a terminal in ASCII. Color monitor required.
The `struct` is great explanation for understanding
the GIF headers.

