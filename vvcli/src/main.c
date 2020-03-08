#include <stdlib.h>
#include "VV.h"

int main(int argc, char** argv)
{
	vv_context* context = NULL;
	vv_context_create(&context);
	vv_context_destroy(&context);
	return EXIT_SUCCESS;
}