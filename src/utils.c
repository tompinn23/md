#include "utils.h"

#include <stdarg.h>

void _debug_print(const char* function, int line, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	printf("[\x1b[94mDEBUG\x1b[0m] \x1b[90m%s:%d\x1b[0m :: ", function, line);
	vprintf(fmt, args);
	printf("\n");
	va_end(args);
}

