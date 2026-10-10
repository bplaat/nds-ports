#pragma once

#include <stddef.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define RAND_MAX 0x7fffffff

void* malloc(size_t size);
void* calloc(size_t count, size_t size);
void* realloc(void* pointer, size_t size);
void* aligned_alloc(size_t alignment, size_t size);
void free(void* pointer);
int abs(int value);
long labs(long value);
int atoi(const char* str);
long strtol(const char* restrict str, char** restrict end, int base);
unsigned long strtoul(const char* restrict str, char** restrict end, int base);
void qsort(void* base, size_t count, size_t size, int (*compare)(const void*, const void*));
void* bsearch(const void* key, const void* base, size_t count, size_t size, int (*compare)(const void*, const void*));
int rand(void);
void srand(unsigned seed);
// There is no environment
static inline char* getenv(const char* name) {
    (void)name;
    return NULL;
}
int atexit(void (*function)(void));
void exit(int status) __attribute__((noreturn));
void abort(void) __attribute__((noreturn));
