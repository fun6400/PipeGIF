/*
vim: ts=4 expandtab :

 The ansiTerminal is a text mode virtual display.
 Instead of writing to a graphical display it writes to a ANSI text screen with ANSI escape codes.

*/

#ifndef ANSI_TERMINAL_H
#define ANSI_TERMINAL_H

#include "../src/VirtualDisplay.h"

class ansiTerminal : public VirtualDisplay {
public:
    ansiTerminal(uint16_t maxColumns, uint16_t maxRows);
    void putPixel(uint8_t _colorIndex);
    void imageEnded() override;
    void clearScreen() override;
    void setColorTable(uint8_t intensity) override;

    uint8_t colorTable[3 * 256];

private:
    void gotoXY() override;

    uint16_t maxColumns;
    uint16_t maxRows;
    uint16_t colorTableIndex; // Safely tracks color table population
};

#endif // ANSI_TERMINAL_H
