#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>
#include "bmp.h"


struct BmpFileHeader
{
	int16_t		bfType;
	uint32_t	bfSize;
	uint16_t	__bfReserved1;
	uint16_t	__bfReserved2;
	uint32_t	bfOffBits;
} __attribute__ ((packed));

struct BmpImageHeader
{
	uint32_t	biSize;
	int32_t		biWidth;
	int32_t		biHeight;
	uint16_t	biPlanes;
	uint16_t	biBitCount;
	uint32_t	biCompression;
	uint32_t	biSizeImage;
	int32_t		biXPelsPerMeter;
	int32_t		biYPelPerMeter;
	uint32_t	biClrUsed;
	uint32_t	biClrImportant;
} __attribute__ ((packed));

int write_pixels_to_bmp(const char* bmp_file, const int width, const int height, const char* image)
{
	FILE* fp = NULL;
	struct BmpFileHeader	file_header;
	struct BmpImageHeader	image_header;

	fp = fopen(bmp_file, "w");

	memset(&file_header, 0, sizeof(struct BmpFileHeader));
	memset(&image_header, 0, sizeof(struct BmpImageHeader));

	file_header.bfType = 0x4d42;
	file_header.bfOffBits = sizeof(struct BmpFileHeader) + sizeof(struct BmpImageHeader);
	file_header.bfSize = file_header.bfOffBits + width * height * 4;

	image_header.biSize = sizeof(struct BmpImageHeader);
	image_header.biWidth = width;
	image_header.biHeight = height;
	image_header.biBitCount = 32;
	image_header.biXPelsPerMeter = 2952;
	image_header.biYPelPerMeter = 2952;
	image_header.biPlanes = 1;
	image_header.biSizeImage = width * height * 4;

	fwrite(&file_header, 1, sizeof(struct BmpFileHeader), fp);
	fwrite(&image_header, 1, sizeof(struct BmpImageHeader), fp);
	fwrite(image, 1, width * height * 4, fp);

	fclose(fp);

	return 0;
}
