/*
 * VirtualDisplay.h
 * by Hans Schou <hans@schou.dk> © 2026
 * SPDX-License-Identifier: MIT
 */

#ifndef _VIRTUALDISPLAY_H
#define _VIRTUALDISPLAY_H

#include <stdint.h>

#define VIRDISPLAY_OK 0
#define VIRDISPLAY_FULL 1

class VirtualDisplay {
public:
    virtual ~VirtualDisplay() = default;
    virtual void putPixel(uint8_t /*colorIndex*/){}
    virtual void gotoXY(void);
    uint16_t incrementX(void);
    virtual void imageEnded(void);
    virtual void setCanvas(uint16_t width, uint16_t height);
    virtual void setImageLimits(uint16_t width, uint16_t height);
    virtual void setOffset(uint16_t left, uint16_t top);
    virtual void setColorTableEntries(uint16_t colorTableRows);
    virtual void setTransparentColor(uint8_t transparantColor);
    virtual void setColorTable(uint8_t /*intensity*/);
    virtual void setInterlace(uint8_t interlaced);
    virtual void clearScreen(void);
    uint16_t x, y;
    uint16_t left = 0, top = 0;
    uint16_t width, height, maxWidth, maxHeight = 0;
    uint16_t colorIndex = 0;
    uint16_t loop = 0;
    uint8_t transparantColor;
private:
    uint16_t colorTableRows = 0;
    uint8_t interlaced;
    uint8_t pass = 0;
};

#endif /* _VIRTUALDISPLAY_H */

// vim: ts=4 expandtab :
