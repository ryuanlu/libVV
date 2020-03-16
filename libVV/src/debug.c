#include "VV.h"
#include "debug.h"


static const char* vv_error_messages[] =
{
	"VV_SUCCESS",
	"VV_FAILED_TO_INITIALIZE",
	"VV_INVALID_VALUE",
	"VV_INVALID_CONTEXT",
	"VV_OPERATION_NOT_SUPPORTED",
	"VV_BAD_ALLOCATION",
};


void vv_print_error(enum vv_result code, const char* filename, const int line, const char* function)
{
	fprintf(stderr, "%s:%d:\t%s() returns %s\n", filename, line, function, vv_error_messages[code]);
}