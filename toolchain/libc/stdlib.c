#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

int abs(int value) {
    return value < 0 ? -value : value;
}

long labs(long value) {
    return value < 0 ? -value : value;
}

int atoi(const char* str) {
    return (int)strtol(str, NULL, 10);
}

// Parses the digits after the sign, the base 0 means C notation (0x hex, 0 octal or decimal)
static unsigned long parse(const char* str, char** end, int base, bool* negative, bool* overflow) {
    const char* s = str;
    while (isspace((unsigned char)*s))
        s++;
    *negative = *s == '-';
    if (*s == '-' || *s == '+')
        s++;
    if ((base == 0 || base == 16) && s[0] == '0' && (s[1] | 32) == 'x' && isxdigit((unsigned char)s[2])) {
        s += 2;
        base = 16;
    } else if (base == 0) {
        base = s[0] == '0' ? 8 : 10;
    }

    unsigned long value = 0;
    const char* digits = s;
    *overflow = false;
    for (;; s++) {
        int digit;
        if (isdigit((unsigned char)*s))
            digit = *s - '0';
        else if (isalpha((unsigned char)*s))
            digit = (*s | 32) - 'a' + 10;
        else
            break;
        if (digit >= base)
            break;
        if (value > (ULONG_MAX - (unsigned)digit) / (unsigned)base)
            *overflow = true;
        value = value * base + digit;
    }
    if (end)
        *end = (char*)(s == digits ? str : s);
    return value;
}

long strtol(const char* restrict str, char** restrict end, int base) {
    bool negative, overflow;
    unsigned long value = parse(str, end, base, &negative, &overflow);
    if (overflow || value > (negative ? -(unsigned long)LONG_MIN : LONG_MAX)) {
        errno = ERANGE;
        return negative ? LONG_MIN : LONG_MAX;
    }
    return negative ? (long)-value : (long)value;
}

unsigned long strtoul(const char* restrict str, char** restrict end, int base) {
    bool negative, overflow;
    unsigned long value = parse(str, end, base, &negative, &overflow);
    if (overflow) {
        errno = ERANGE;
        return ULONG_MAX;
    }
    return negative ? -value : value;
}

static void swap(char* a, char* b, size_t size) {
    while (size--) {
        char c = *a;
        *a++ = *b;
        *b++ = c;
    }
}

// Moves the element at root down the heap of count elements until its children are smaller
static void sift_down(char* base, size_t root, size_t count, size_t size, int (*compare)(const void*, const void*)) {
    for (size_t child; (child = 2 * root + 1) < count; root = child) {
        if (child + 1 < count && compare(base + child * size, base + (child + 1) * size) < 0)
            child++;
        if (compare(base + root * size, base + child * size) >= 0)
            return;
        swap(base + root * size, base + child * size, size);
    }
}

// Heapsort: in place, no recursion and O(n log n) for every input
void qsort(void* base, size_t count, size_t size, int (*compare)(const void*, const void*)) {
    char* b = base;
    for (size_t i = count / 2; i-- > 0;)
        sift_down(b, i, count, size, compare);
    for (size_t end = count; end-- > 1;) {
        swap(b, b + end * size, size);
        sift_down(b, 0, end, size, compare);
    }
}

void* bsearch(const void* key, const void* base, size_t count, size_t size, int (*compare)(const void*, const void*)) {
    const char* b = base;
    while (count) {
        const char* middle = b + (count / 2) * size;
        int order = compare(key, middle);
        if (order == 0)
            return (void*)middle;
        if (order > 0) {
            b = middle + size;
            count -= count / 2 + 1;
        } else {
            count /= 2;
        }
    }
    return NULL;
}

// xorshift32, the state may never be 0
static uint32_t random_state = 1;

int rand(void) {
    random_state ^= random_state << 13;
    random_state ^= random_state >> 17;
    random_state ^= random_state << 5;
    return (int)(random_state & RAND_MAX);
}

void srand(unsigned seed) {
    random_state = seed ? seed : 1;
}
