#ifndef __RAW_H__
#define __RAW_H__

#include <stdio.h>

int raw_open(FILE* fp, struct volume* volume, const void* raw_params);

#endif /* __RAW_H__ */
