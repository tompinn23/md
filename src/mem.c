#include "mem.h"

#include <string.h>
#include <stdio.h>


void* mem_malloc(size_t sz) {
	char* mem;

	if(sz == 0) return NULL;

	mem = malloc(sz + sizeof(size_t));
	if(!mem) {
		printf("Out of memory!\n");
		exit(EXIT_FAILURE);
	}

	mem += sizeof(size_t);
#if defined(MEM_POISON_ALLOC)
	memset(mem, 0xCC, sz);
#endif
	SZ(mem) = sz;
	return mem;
}

void* mem_zalloc(size_t sz) {
	void* mem = mem_malloc(sz);

	if(sz) {
		memset(mem, 0, sz);
	}
	return mem;
}

void mem_free(const void* ptr) {
	if(!ptr) return;

#if defined(MEM_POISON_FREE)
	memset(ptr, 0xCD, SZ(ptr));
#endif
	free((char*)ptr - sizeof(size_t));
}

void* mem_realloc(void* ptr, size_t sz) {
	char *mem = ptr;
	if(sz == 0) return NULL;

	mem = realloc(mem ? mem - sizeof(size_t) : NULL, sz + sizeof(size_t));
	mem += sizeof(size_t);

	if(!mem) {
		printf("Out of memory!\n");
		exit(EXIT_FAILURE);
	}

	SZ(mem) = sz;
	return mem;
}

char* string_make(const char* src) {
	unsigned len = strlen(src);
	char* new = mem_malloc(len * sizeof(char) + 1);
	
	memcpy(new, src, len);
	new[len] = '\0';
	return new;
}

char* string_concat(const char* dest, const char* src) {
	int sz = snprintf(NULL, 0, "%s%s", dest, src);
	char* str = mem_malloc(sz + 1);
	str[sz] = 0;
	int ret = 0;
	if((ret = snprintf(str, sz, "%s%s", dest, src)) > 0 && ret < sz + 1) {
		return str;
	}
	printf("Error in snprintf: %d\n", ret);
	free(str);
	return NULL;
}

char* string_split(const char* src, const char* delim) {
	char* a = string_make(src);
	char* b = strtok(a, delim);
	char* c = string_make(b);
	mem_free(a);
	return c;
}
