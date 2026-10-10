// A small sscanf: the d, i, u, x, s, c and % conversions with widths, the * flag and the hh, h,
// l and ll lengths, spaces in the format skip any spaces

#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int vsscanf(const char* restrict str, const char* restrict format, va_list args) {
    const char* s = str;
    int assigned = 0;
    for (const char* c = format; *c; c++) {
        if (isspace((unsigned char)*c)) {
            while (isspace((unsigned char)*s))
                s++;
            continue;
        }
        if (*c != '%' || c[1] == '%') {
            c += *c == '%';
            if (*s != *c)
                break;
            s++;
            continue;
        }
        c++;
        bool skip = *c == '*';
        c += skip;
        int width = 0;
        for (; isdigit((unsigned char)*c); c++)
            width = width * 10 + (*c - '0');
        int size = 0;  // -2 char, -1 short, 1 long long
        for (; *c == 'h' || *c == 'l'; c++)
            size += *c == 'h' ? -1 : c[1] == 'l';
        if (*c != 'c') {
            while (isspace((unsigned char)*s))
                s++;
        }
        if (!*s)
            break;

        if (*c == 's' || *c == 'c') {
            if (!width)
                width = *c == 'c' ? 1 : INT32_MAX;
            char* out = skip ? NULL : va_arg(args, char*);
            int length = 0;
            for (; length < width && s[length] && (*c == 'c' || !isspace((unsigned char)s[length])); length++) {
                if (out)
                    out[length] = s[length];
            }
            if (out && *c == 's')
                out[length] = '\0';
            s += length;
        } else if (*c == 'd' || *c == 'i' || *c == 'u' || *c == 'x' || *c == 'X') {
            char buffer[24];
            int length = 0;
            if (!width || width > (int)sizeof(buffer) - 1)
                width = sizeof(buffer) - 1;
            for (; length < width && s[length] && !isspace((unsigned char)s[length]); length++)
                buffer[length] = s[length];
            buffer[length] = '\0';
            char* end;
            int base = *c == 'x' || *c == 'X' ? 16 : *c == 'i' ? 0 : 10;
            long value = *c == 'd' || *c == 'i' ? strtol(buffer, &end, base) : (long)strtoul(buffer, &end, base);
            if (end == buffer)
                break;
            s += end - buffer;
            if (!skip) {
                void* out = va_arg(args, void*);
                if (size <= -2)
                    *(char*)out = (char)value;
                else if (size == -1)
                    *(short*)out = (short)value;
                else if (size >= 1)
                    *(long long*)out = value;
                else
                    *(int*)out = (int)value;
            }
        } else {
            break;
        }
        assigned += !skip;
    }
    return assigned;
}

int sscanf(const char* restrict str, const char* restrict format, ...) {
    va_list args;
    va_start(args, format);
    int assigned = vsscanf(str, format, args);
    va_end(args);
    return assigned;
}
