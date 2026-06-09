/*
 * VirtualDisplay.cpp
 * by Hans Schou <hans@schou.dk> © 2026
 * SPDX-License-Identifier: MIT
 */

//#include <stdint.h>
#include "VirtualDisplay.h"

// Increment X from left to right.
// If interlaced, advance Y with an algorithm.
uint16_t VirtualDisplay::incrementX(void) {
    if (++x == width) {
        x = 0;
        if (!interlaced) {
            ++y;
        } else {
            static const uint8_t InterlacedOffset[] = { 0, 4, 2, 1 };
            static const uint8_t InterlacedJumps[] = { 8, 8, 4, 2 };
            y += InterlacedJumps[pass];
            if (y >= height) {
                ++pass;
                pass %= sizeof(InterlacedOffset)/sizeof(InterlacedOffset[0]);
                y = InterlacedOffset[pass];
            }
        }
        if (y >= height) {
            y %= height;
        }
    }
    return x;
}

void VirtualDisplay::imageEnded(void) {}

void VirtualDisplay::gotoXY(void) {}

void VirtualDisplay::setCanvas(uint16_t width, uint16_t height) {
    this->maxWidth = width;
    this->maxHeight = height;
}

void VirtualDisplay::setImageLimits(uint16_t width, uint16_t height) {
    this->width = width;
    this->height = height;
    if (width > maxWidth)
        width = maxWidth;
    if (height > maxHeight)
        height = maxHeight;
}

void VirtualDisplay::setOffset(uint16_t left, uint16_t top) {
    this->left = left;
    this->top = top;
}

void VirtualDisplay::setColorTableEntries(uint16_t colorTableRows) {
    this->colorTableRows = colorTableRows;
    colorIndex = 0;
}

void VirtualDisplay::setTransparentColor(uint8_t transparantColor) {
    this->transparantColor = transparantColor;
}

void VirtualDisplay::setColorTable(uint8_t /*intensity*/) {}

void VirtualDisplay::setInterlace(uint8_t interlaced) {
    this->interlaced = interlaced;
}

void VirtualDisplay::clearScreen(void) {
    x = 0;
    y = 0;
}

// vim: ts=4 expandtab :
