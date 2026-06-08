/*
 * At least 17KB SRAM is required for LzwDecode.
 */

#ifndef _LZWDECODE_H
#define _LZWDECODE_H

#include "VirtualDisplay.h"

#define lzwMaxBits 12
#define LZW_SIZTABLE (1 << lzwMaxBits)

#define LZW_OK 0
#define LZW_END_CODE 1
#define LZW_CODE_GE_SLOT -11
#define LZW_CURSIZE_GE_MAXBITS -12
#define LZW_ERROR_CODESIZE -13
#define LZW_ERROR_OVERFLOW -14
#define LZW_ERROR_INVALID -15

class LzwDecode {
public:
    LzwDecode(VirtualDisplay* display);
    int decode(uint16_t nextByte);
    int initCodeSize(uint8_t initialCodeSize);
private:
    VirtualDisplay* display;
    uint8_t bitsInBuffer;
    uint32_t bitBuffer;
    uint8_t currentCodeSize;
    uint16_t currentMask;
    int codeSize;
    int clearCode;
    int endCode;
    int newCodes;
    int maxCode;
    int nextFreeSlot;
    int firstCode, previousCode;
    uint16_t stackIndex;
    uint8_t stack[LZW_SIZTABLE];
    uint8_t suffix[LZW_SIZTABLE];
    uint16_t prefix[LZW_SIZTABLE];
    unsigned int mask[17] = {
        0x0000, 0x0001, 0x0003, 0x0007,
        0x000F, 0x001F, 0x003F, 0x007F,
        0x00FF, 0x01FF, 0x03FF, 0x07FF,
        0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF,
        0xFFFF
    };
};

#endif
