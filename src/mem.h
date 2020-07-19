#pragma once

#include <stdlib.h>

#define SZ(uptr) 	*((size_t *)((char *)(uptr) - sizeof(size_t)))


void* mem_malloc(size_t sz);
void* mem_zalloc(size_t sz);
void* mem_realloc(void* p, size_t len);
void mem_free(const void* ptr);

char* string_make(const char* src);
char* string_concat(const char* dest, const char* src);
char* string_split(const char* src, const char* delim);
