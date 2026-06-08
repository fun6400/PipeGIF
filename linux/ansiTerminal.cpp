#include "ansiTerminal.h"
#include <stdio.h> // Required for printf and fflush
#include <stdlib.h>

constexpr int XFACTOR = 2;
constexpr int TERM_X = 1;
constexpr int TERM_Y = 1;

ansiTerminal::ansiTerminal(uint16_t maxColumns, uint16_t maxRows) {
    printf("\e[2J\e[H");
    this->maxColumns = maxColumns - 1;
    this->maxRows = maxRows - 1;
    this->colorTableIndex = 0;
}

void ansiTerminal::putPixel(uint8_t _colorIndex) {
    if (x * XFACTOR < maxColumns && y < maxRows) {
        if (_colorIndex != colorIndex) { // Has color changed?
            printf("\e[48;2;%d;%d;%dm",
                colorTable[_colorIndex * 3 + 0],
                colorTable[_colorIndex * 3 + 1],
                colorTable[_colorIndex * 3 + 2]);
            colorIndex = _colorIndex;
        }
        if (_colorIndex == transparantColor) {
            printf("\e[%dC", XFACTOR); /* move forward */
        } else {
            printf("%*s", XFACTOR, ""); /* draw XFACTOR pixels on screen */
        }
    }

    if (0 == incrementX()) {
        gotoXY();
    }

    uint8_t r = (x == width && y == height) ? VIRDISPLAY_FULL : VIRDISPLAY_OK;
    
    if (VIRDISPLAY_FULL == r) {
        printf("\e[0m");
    }
}

void ansiTerminal::gotoXY(void) {
    if  (y > 50 && x > 150) {
	printf("ERROR e[%d;%dH", TERM_Y + y + top, TERM_X + XFACTOR * (x + left));
	exit(1);
    }
    printf("\e[%d;%dH", TERM_Y + (y + top), TERM_X + XFACTOR * (x + left));
}

void ansiTerminal::imageEnded(void) {
    printf("\e[0m\e[H");
    //printf("\e[?25h"); // show cursor
}

void ansiTerminal::clearScreen() {
    //VirtualDisplay::clearScreen();
    printf("\e[2J");
    //printf("\e[?25l"); // hide cursor
}

void ansiTerminal::setColorTable(uint8_t intensity) {
    if (colorTableIndex < sizeof(colorTable)) {
        colorTable[colorTableIndex++] = intensity;
    }
}
