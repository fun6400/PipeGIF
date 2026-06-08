/*
 * PipeGIF.h
 * by Hans Schou <hans@schou.dk> © 2026
 * SPDX-License-Identifier: MIT
 */

#ifndef PipeGIF_h
#define PipeGIF_h

#include <memory>
#include "LzwDecode.h"
#include "VirtualDisplay.h"

#define GIF_OK 0
#define GIF_TRAILER 1
#define GIF_DELAY100 2
#define GIF_ERROR_NOT_GIF_HEADER -1
#define GIF_ERROR_NOT_89A -2
#define GIF_ERROR_UNKNOWN_EXTENSION -3
#define GIF_ERROR_UNKNOWN_EXTENSION_LABEL -4
#define GIT_ERROR_EXPECTED_BLOCK_TERMINATOR -5
#define GIF_ERROR_UNDEF_STATE -6
#define GIF_ERROR_GEC_WRONG_SIZE -7
#define GIF_ERROR_MIS_NULL_TERM -8
#define GIF_ERROR_PLAIN_TEXT_BLOCK_WRONG -9
#define GIF_ERROR_APPSUB_OVERSIZE -10

enum states {
/*  0 */ Start = 0,
/*  1 */ getLogicalScreenDescriptor,
/*  2 */ getGlobalColorTable,
/*  3 */ getExtension,
/*  4 */ getExtensionLabel,
/*  5 */ getImageDesciptor,
/*  6 */ getGraphicControlExtension,
/*  7 */ getComment,
/*  8 */ getApplicationExtension,
/*  9 */ getLocalColorMap,
/* 10 */ getLzwCodeSize,
/* 11 */ getLzwBlockSize,
/* 12 */ getLzwData,
/* 13 */ getPlainTextExtension,
/* 14 */ getApplicationExtensionSub,
/* 15 */ getPlainTextExtensionBlock,
/* 16 */ getPlainTextExtensionData,
};

class PipeGIF {
public:
    PipeGIF(VirtualDisplay *display);
    ~PipeGIF();
    int decode(uint8_t newData);
    uint16_t getDelay100(void);
    uint16_t getLoop(void);
private:
    states initStateStart(void);
    LzwDecode lzw;
    VirtualDisplay *display;
    states state;
    char gifVersion;
    unsigned int localSize;
    uint8_t *localPointer;
    unsigned int colorTableEntries;
#pragma pack(push,1)
    struct {
        uint16_t screenWidth;
        uint16_t screenHeight;
        struct {
            unsigned int sizeGlobalColorTable : 3;
            unsigned int sortFlag : 1;
            unsigned int colorResolution : 3;
            unsigned int globalColorTable : 1;
        } packed;
        uint8_t backgroundColorIndex;
        uint8_t aspectRatio;
    } logicalScreenDescriptor;
    struct {
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
        char netscape[8];
        char version[3];
    } applicationExtension;
    struct {
        uint8_t mark; // = 0x01
        uint16_t loop; // 0..65535 lo-hi format
        uint8_t terminator;
    } applicationExtensionSub;
#pragma pack(pop)
};

#endif
// vim: ts=4 expandtab :
