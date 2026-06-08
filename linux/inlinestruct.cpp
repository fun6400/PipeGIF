/*
 * inlinestruct.cpp
 * by Hans Schou <hans@schou.dk> © 2026
 * SPDX-License-Identifier: MIT
 *
 * Test run from command line: clear ; ./inlinestruct ; sleep 2
 */

#include <stdlib.h> // EXIT_SUCCESS

#include "../src/PipeGIF.h"
#include "ansiTerminal.h"

#pragma pack(push,1)
struct {
    struct {
        char signature[3];
        char version[3];
    } header;
    struct {
        uint16_t width;
        uint16_t height;
        struct {
            unsigned int sizeGlobalColorTable : 3;
            unsigned int sortFlag : 1;
            unsigned int colorResolution : 3;
            unsigned int globalColorTable : 1;
        } packed;
        uint8_t backgroundColorIndex;
        uint8_t aspectRatio;
    } logicalScreenDescriptor;
    uint8_t globalColorTable[ 3 * (1 << (1+1)) ]; // 12 = 3 * 2^(1 + sizeGlobalColorTable)
    struct {
        uint8_t introducer;
        uint8_t label;
        uint8_t blockSize;
        struct {
            unsigned int transparentColorFlag : 1;
            unsigned int userInputFLag : 1;
            unsigned int disposalMethod : 3;
            unsigned int reserved : 3;
        } packed;
        uint16_t delay100;
        uint8_t transparentColorIndex;
        uint8_t blockTerminator;
    } graphicControlExtension;
    struct {
        uint8_t introducer;
        uint8_t label;
        char data[6]; // Hello
    } comment;
    struct {
        uint8_t imageSeparator;
        uint16_t left;
        uint16_t top;
        uint16_t width;
        uint16_t height;
        struct {
            unsigned int sizeLocalColorTable : 3;
            unsigned int reserved : 2;
            unsigned int sort : 1;
            unsigned int interlaced : 1;
            unsigned int localColorTable : 1;
        } packed;
    } imageDescriptor;
    struct {
        uint8_t codeSize;
        struct {
            uint8_t blockSize;
            uint8_t data[22];
        } block0;
        uint8_t blockTerminator;
    } lzw;
    uint8_t trailer;
} constexpr inline const imageStruct = {
    .header = {
        .signature = {'G','I','F'},
        .version = {'8','9','a'},
    },
    .logicalScreenDescriptor = {
        .width = 10,
        .height = 10,
        .packed = {
            .sizeGlobalColorTable = 1, // 4 = 2^(1 + 1)
            .sortFlag = 0,
            .colorResolution = 4,
            .globalColorTable = 1,
        },
        .backgroundColorIndex = 0,
        .aspectRatio = 0,
    },
    .globalColorTable = {
        0xff, 0xff, 0xff, // White
        0xff, 0x00, 0x00, // Red
        0x00, 0x00, 0xff, // Blue, try yellow: 0xff, 0xff, 0x00
        0x00, 0x00, 0x00, // not used, filler
    },
    .graphicControlExtension = {
        .introducer = 0x21,
        .label = 0xf9,
        .blockSize = 4,
        .packed = {
            .transparentColorFlag = 0,
            .userInputFLag = 0,
            .disposalMethod = 1,
            .reserved = 0,
        },
        .delay100 = 0,
        .transparentColorIndex = 0,
        .blockTerminator = 0,
    },
    .comment = {
        .introducer = 0x21,
        .label = 0xfe,
        .data = "Hello", // If changed, also change definition above to strlen()+1 = 6
    },
    .imageDescriptor = {
        .imageSeparator = 0x2c,
        .left = 20, // Try change this offset to 10
        .top = 9,   // Try change this offset to 2
        .width = 10,
        .height = 10,
        .packed = {
            .sizeLocalColorTable = 0,
            .reserved = 0,
            .sort = 0,
            .interlaced = 0,
            .localColorTable = 0,
        },
    },
    .lzw = {
        .codeSize = 2,
        .block0 = {
            .blockSize = sizeof(imageStruct.lzw.block0.data),
            .data = {
                0x8c, 0x2d, 0x99, 0x87, 0x2a, 0x1c, 0xdc, 0x33,
                0xa0, 0x02, 0x75, 0xec, 0x95, 0xfa, 0xa8, 0xde,
                0x60, 0x8c, 0x04, 0x91, 0x4c, 0x01,
            },
        },
        .blockTerminator = 0,
    },
    .trailer = 0x3b,
};
#pragma pack(pop)

int main(void) {
    long unsigned int i;

    ansiTerminal ansiTerm(80, 25); // Text mode demo display
    PipeGIF gif( &ansiTerm ); // Decode GIF and send it to display

    const unsigned char *p = reinterpret_cast<const unsigned char*>(&imageStruct);
    for (i=0; i<sizeof(imageStruct); i++)
       gif.decode(*p++);

    return EXIT_SUCCESS;
}
