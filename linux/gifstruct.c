
#include <stdio.h>
#include <stdint.h>

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
# error "This program does only support little-endian architectures."
#endif

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
} const imageStruct = {
    .header = {
        .signature = {'G','I','F'},
        .version = {'8','9','a'},
    },
    .logicalScreenDescriptor = {
        .width = 10,
        .height = 10,
        .packed = {
            .sizeGlobalColorTable = 3, // 16 = 2^(1 + 3)
            .sortFlag = 0,
            .colorResolution = 4,
            .globalColorTable = 1,
        },
        .backgroundColorIndex = 0,
        .aspectRatio = 0,
    },
};
#pragma pack(pop)

int main(void) {
	printf("imageStruct(%ld)\n", sizeof(imageStruct));
	printf("  .header(%ld)\n", sizeof(imageStruct.header));
	printf("    .signature(%ld): %.*s\n",
		sizeof(imageStruct.header.signature),
		(int)sizeof(imageStruct.header.signature),
		imageStruct.header.signature);
	printf("    .version(%ld): %.*s\n",
		sizeof(imageStruct.header.version),
        	(int)sizeof(imageStruct.header.version),
        	imageStruct.header.version);
        printf("  .logicalScreenDescriptor(%ld)\n",
		sizeof(imageStruct.logicalScreenDescriptor));
        printf("    .width(%ld): %d\n",
		sizeof(imageStruct.logicalScreenDescriptor.width),
        	imageStruct.logicalScreenDescriptor.width);
	printf("    .height(%ld): %d\n",
		sizeof(imageStruct.logicalScreenDescriptor.height),
		imageStruct.logicalScreenDescriptor.height);
	printf("    .packed(%ld)\n",
		sizeof(imageStruct.logicalScreenDescriptor.packed));
	printf("      .sizeGlobalColorTable(?): %d\n",
		imageStruct.logicalScreenDescriptor.packed.sizeGlobalColorTable);
	printf("      .sortFlag: %d\n",
		imageStruct.logicalScreenDescriptor.packed.sortFlag);
	printf("      .colorResolution: %d\n",
		imageStruct.logicalScreenDescriptor.packed.colorResolution);
	printf("      .globalColorTable: %d\n",
		imageStruct.logicalScreenDescriptor.packed.globalColorTable);
	printf("    .backgroundColorIndex(%ld): %d\n",
		sizeof(imageStruct.logicalScreenDescriptor.backgroundColorIndex),
		imageStruct.logicalScreenDescriptor.backgroundColorIndex);
	printf("    .aspectRatio(%ld): %d\n",
		sizeof(imageStruct.logicalScreenDescriptor.aspectRatio),
		imageStruct.logicalScreenDescriptor.aspectRatio);
	return 0;
}
