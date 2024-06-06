#ifndef __BMP_H__
#define __BMP_H__

#include <stdint.h>

int write_pixels_to_bmp(const char* bmp_file, const int width, const int height, const char* image);

#endif /* __BMP_H__ */