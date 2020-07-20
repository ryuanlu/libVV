#ifndef __DEBUG__
#define __DEBUG__


#include <stdio.h>
#include "VV.h"

void vv_print_error(const enum vv_result code, const char* filename, const int line, const char* function);

#define goto_cleanup_if_failed(__result__, __goto_label__) {result = __result__; if(result != VV_SUCCESS) { vv_print_error(result, __FILE__, __LINE__, __FUNCTION__); goto __goto_label__; }}
#define goto_cleanup_if(__expr__, __result__, __goto_label__) goto_cleanup_if_failed((__expr__) ? __result__ : VV_SUCCESS, __goto_label__ )

#endif /* __DEBUG__ */