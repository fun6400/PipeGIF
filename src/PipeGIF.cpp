/*
 * PipeGIF.cpp
 * by Hans Schou <hans@schou.dk> © 2026
 * SPDX-License-Identifier: MIT
 */

//#include <stdio.h>
#include <stdint.h> // uint8_t
#include <stdlib.h>
#include <string.h> // memset()

#include <sys/types.h>
#include <time.h>

#include "PipeGIF.h"
#include "VirtualDisplay.h"
#include "LzwDecode.h"

#include <stddef.h>
#include <ctype.h>

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
# error "This program does not support big-endian architectures."
# error "Data are read directly into an expected C type little endian struct."
#endif

void dump(const char *str, const void *p, size_t size) {
    if (!str || !p) return;
    const unsigned char *buffer = (const unsigned char *)p;
    printf("\e[0m%s: ", str);
    for (size_t i = 0; i < size; i++) {
        unsigned char ch = buffer[i];
        char display_ch = isprint(ch) ? (char)ch : '.';
        printf("'%c' 0x%02X%s", display_ch, ch, (i == size - 1) ? "" : ", ");
    }
    printf("\n");
    _exit(1);
}

static const char textGIF87a[] = { 'G','I','F','8','7','a' };
#define GIF_VERSION_POS 4 /*        0   1   2   3   ^      */

PipeGIF::PipeGIF(VirtualDisplay *display) : lzw( display ) {
    this->display = display;
    state = initStateStart();
}

PipeGIF::~PipeGIF() {
}

int PipeGIF::decode(uint8_t newData) {
    int result = GIF_OK;
    switch (state) {
        /* Let getLzwData be the first case to parse faster */
        case getLzwData:
            if (!--localSize) {
                state = getLzwBlockSize;
            }
            //if (LZW_END_CODE == lzw->decode(newData)) {
            if (LZW_END_CODE == lzw.decode(newData)) {
                state = getLzwBlockSize;
                display->imageEnded();
                if (graphicControlExtension.delay100) {
                    result = GIF_DELAY100;
                }
            }
            break;
        /* getExtension is called 7 times, make it second state */
        case getExtension:
            switch (newData) {
                case 0x21: // Extension introducer
                    state = getExtensionLabel;
                    break;
                case 0x2c: // Image Descriptor
                    state = getImageDesciptor;
                    localSize = sizeof(imageDescriptor);
                    localPointer = (uint8_t *)&imageDescriptor;
                    break;
                case 0x3b: // Trailer
                    state = initStateStart();
                    display->imageEnded();
                    result = GIF_TRAILER;
                    break;
                default: 
                    result = GIF_ERROR_UNKNOWN_EXTENSION;
            }
            break;
        /* Start of the GIF file/datastream */
        case Start:
            if (textGIF87a[sizeof(textGIF87a)-localSize] != newData) {
                if (sizeof(textGIF87a)-GIF_VERSION_POS != localSize) {
                    result = GIF_ERROR_NOT_GIF_HEADER;
                } else {
                    if ('9' != newData) {
                        result = GIF_ERROR_NOT_89A;
                    }
                    gifVersion = newData;
                }
            }
            if (!--localSize) {
                state = getLogicalScreenDescriptor;
                localSize = sizeof(logicalScreenDescriptor);
                localPointer = (uint8_t *)&logicalScreenDescriptor;
            }
            break;
        case getLogicalScreenDescriptor:
            *localPointer++ = newData;
            if (!--localSize) {
                display->setCanvas(logicalScreenDescriptor.screenWidth, logicalScreenDescriptor.screenHeight);
                if (! logicalScreenDescriptor.packed.globalColorTable)
                    state = getExtension;
                else {
                    state = getGlobalColorTable;
                    colorTableEntries = 1 << (1+logicalScreenDescriptor.packed.sizeGlobalColorTable);
                    localSize = 3 * colorTableEntries;
                    display->setColorTableEntries(colorTableEntries);
                }
            }
            break;
        case getGlobalColorTable:
            display->setColorTable(newData);
            if (!--localSize) {
                state = getExtension;
            }
            break;
        case getLocalColorMap:
            display->setColorTable(newData);
            if (!--localSize) {
                state = getLzwCodeSize;
            }
            break;
        case getExtensionLabel:
            localSize = 0;
            switch (newData) {
                case 0x01: // Plain text label
                    state = getPlainTextExtension;
                    break;
                case 0xF9: // Graphic Control Extension
                    state = getGraphicControlExtension;
                    localSize = sizeof(graphicControlExtension);
                    localPointer = (uint8_t *)&graphicControlExtension;
                    break;
                case 0xFE: // Comment Extension
                    state = getComment;
                    break;
                case 0xFF: // Application Extension
                    state = getApplicationExtension;
                    localSize = 0;
                    localPointer = (uint8_t *)&applicationExtension;
                    break;
                default:
                    result = GIF_ERROR_UNKNOWN_EXTENSION_LABEL;
            }
            break;
        case getImageDesciptor:
            *localPointer++ = newData;
            if (!--localSize) {
                display->setImageLimits(imageDescriptor.width, imageDescriptor.height);
                display->setOffset(imageDescriptor.left, imageDescriptor.top);
                display->setInterlace(imageDescriptor.packed.interlaced);
                display->x = 0;
                display->y = 0;
                display->gotoXY();
                if (!imageDescriptor.packed.localColorTable) {
                    state = getLzwCodeSize;
                } else {
                    state = getLocalColorMap;
                    colorTableEntries = 1 << (1+imageDescriptor.packed.sizeLocalColorTable);
                    localSize = 3 * colorTableEntries;
                    display->setColorTableEntries(colorTableEntries);
                }
            }
            break;
        case getApplicationExtension:
            if (0 == localSize) {
                localSize = newData;
            } else {
                *localPointer++ = newData;
                if (!--localSize) {
                    state = getApplicationExtensionSub;
                    localSize = 0;
                    localPointer = (uint8_t *)&applicationExtensionSub;
                }
            }
            break;
        case getComment:
            if (0 == newData) {
                state = getExtension;
            } else {
                // Do something with comment data in variable newData.
            }
            break;
        case getPlainTextExtension:
            // TODO: Plain Text Extension
            // First newData is block size == 12
            // Followed by a null terminated ASCII string
            if (12 != newData) {
                result = GIF_ERROR_PLAIN_TEXT_BLOCK_WRONG;
            } else {
                state = getPlainTextExtensionBlock;
                localSize = newData;
            }
            break;
        case getPlainTextExtensionBlock:
            if (0 == --localSize) {
                state = getPlainTextExtensionData;
            } else {
                // Do something with comment data in variable newData.
            }
            break;
        case getPlainTextExtensionData:
            if (0 == newData) {
                state = getExtension;
            } else {
                // Do something with comment data in variable newData.
            }
            break;
        case getApplicationExtensionSub:
            if (0 == localSize) {
                localSize = newData+1;
                if (localSize >= sizeof(applicationExtensionSub)) {
                    result = GIF_ERROR_APPSUB_OVERSIZE;
                }
            } else {
                *localPointer++ = newData;
                if (!--localSize) {
//printf("applicationExtensionSub.loop %d\n", applicationExtensionSub.loop );
//dump("applicationExtensionSub", &applicationExtensionSub, sizeof(applicationExtensionSub));
                    display->loop = applicationExtensionSub.loop;
                    if (0 == newData) {
                        state = getExtension;
                    } else {
                        result = GIF_ERROR_MIS_NULL_TERM;
                    }
                }
            }
            break;
        case getGraphicControlExtension:
            *localPointer++ = newData;
            if (!--localSize) {
                state = getExtension;
                display->setTransparentColor(graphicControlExtension.transparentColorIndex);
                if (4 != graphicControlExtension.blockSize && 0 != graphicControlExtension.blockTerminator) {
                    result = GIF_ERROR_GEC_WRONG_SIZE;
                }
            }
            break;
        case getLzwCodeSize:
            lzw.initCodeSize(newData);
            state = getLzwBlockSize;
            break;
        case getLzwBlockSize:
            if (0 == newData) {
                state = getExtension;
            } else {
                state = getLzwData;
                localSize = newData;
            }
            break;
        default:
            result = GIF_ERROR_UNDEF_STATE;
    }
    return result;
}

states PipeGIF::initStateStart(void) {
    state = Start;
    localPointer = NULL;
    localSize = sizeof(textGIF87a);
    gifVersion = textGIF87a[GIF_VERSION_POS];
    colorTableEntries = 0;
    memset(&logicalScreenDescriptor, 0, sizeof(logicalScreenDescriptor));
    memset(&graphicControlExtension, 0, sizeof(graphicControlExtension));
    memset(&imageDescriptor, 0, sizeof(imageDescriptor));
    memset(&applicationExtension, 0, sizeof(applicationExtension));
    memset(&applicationExtensionSub, 0, sizeof(applicationExtensionSub));
    display->x = 0; display->y = 0;
    return state;
}

uint16_t PipeGIF::getDelay100(void) {
    return graphicControlExtension.delay100;
}

uint16_t PipeGIF::getLoop(void) {
    return applicationExtensionSub.loop;
//printf("applicationExtensionSub.loop %d\n", applicationExtensionSub.loop );
}

// vim: ts=4 expandtab :
