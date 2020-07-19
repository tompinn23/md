#pragma once


#include <stdio.h>

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define MD_PRINTF(fmt_idx, first_idx) __attribute__((format(printf, (fmt_idx), (first_idx))))
#else
#define MD_PRINTF
#endif



void _debug_print(const char* function, int line, const char* fmt, ...) MD_PRINTF(3, 4); 

#if defined(NDEBUG)
#define d_print(...)
#else
#define d_print(...) _debug_print(__FUNCTION__, __LINE__, __VA_ARGS__)
#endif
