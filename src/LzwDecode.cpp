/*
 * LzwDecode.cpp
 * by Hans Schou <hans@schou.dk> © 2026
 * SPDX-License-Identifier: MIT
 */

#include <stdint.h>
#include "VirtualDisplay.h"
#include "LzwDecode.h"

// Debugging (remove for production)
#include <unistd.h>
#include <stdlib.h>

// Constructor: Initializes the LZW decoder with a VirtualDisplay
LzwDecode::LzwDecode(VirtualDisplay* display) : display(display) {}

// Initializes the decoder with the initial code size
int LzwDecode::initCodeSize(uint8_t initialCodeSize) {
    if (initialCodeSize > 8) {
        return LZW_ERROR_CODESIZE; // Invalid code size
    }

    // Initialize bit buffer
    bitBuffer = 0;
    bitsInBuffer = 0;

    // Initialize decoder state
    codeSize = initialCodeSize;
    clearCode = 1 << initialCodeSize;
    currentCodeSize = initialCodeSize + 1;
    currentMask = mask[currentCodeSize];
    maxCode = 1 << currentCodeSize;
    endCode = clearCode + 1;
    nextFreeSlot = newCodes = clearCode + 2;
    firstCode = previousCode = -1;
    stackIndex = 0;

    return LZW_OK;
}

// Decodes the next 8 bits of compressed data
int LzwDecode::decode(uint16_t nextByte) {
    bitBuffer |= static_cast<uint32_t>(nextByte) << bitsInBuffer;
    bitsInBuffer += 8;

    while (bitsInBuffer >= currentCodeSize) {
        uint16_t currentCode = bitBuffer & currentMask;
        bitBuffer >>= currentCodeSize;
        bitsInBuffer -= currentCodeSize;

        if (currentCode == endCode) {
            return LZW_END_CODE;
        } 
        
        if (currentCode == clearCode) {
            currentCodeSize = codeSize + 1;
            currentMask = mask[currentCodeSize];
            maxCode = 1 << currentCodeSize;
            nextFreeSlot = newCodes;
            previousCode = -1;
            firstCode = -1;
            stackIndex = 0;
            continue;
        }

        int code = currentCode;

        // Handle the special KwKwK case
        if (code == nextFreeSlot) {
            if (firstCode < 0) return LZW_ERROR_INVALID; // Safety check
            if (stackIndex >= 4096) return LZW_ERROR_OVERFLOW;
            stack[stackIndex++] = firstCode;
            code = previousCode;
        } else if (code > nextFreeSlot) {
            return LZW_CODE_GE_SLOT;
        }

        // Unpack the prefix/suffix chains
        while (code >= newCodes) {
            if (stackIndex >= 4096) return LZW_ERROR_OVERFLOW;
            stack[stackIndex++] = suffix[code];
            code = prefix[code];
        }
        firstCode = code;
        if (stackIndex >= 4096) return LZW_ERROR_OVERFLOW;
        stack[stackIndex++] = firstCode;

        // Output decoded bytes
        while (stackIndex > 0) {
            display->putPixel(stack[--stackIndex]);
        }

        // Update the dictionary
        if (previousCode >= 0 && nextFreeSlot < 4096) {
            suffix[nextFreeSlot] = firstCode;
            prefix[nextFreeSlot] = previousCode;
            nextFreeSlot++;
            // GIF Spec Timing Fix:
            // Increase code size strictly when nextFreeSlot == maxCode, 
            // EXCEPT when we've already hit the 12-bit ceiling.
            if (nextFreeSlot == maxCode && currentCodeSize < 12) {
                currentCodeSize++;
                maxCode = 1 << currentCodeSize;
                currentMask = mask[currentCodeSize];
            }
        }

        previousCode = currentCode;
    }

    return LZW_OK;
}
