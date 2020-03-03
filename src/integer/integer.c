#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include "../error.h"

int longParse(const char *input, char base, long *output) {
    char *end;
    if(base < 2 || base > 36) return STATUS_INPUT_ERR;
    *output = strtol(input, &end, base);
    if(end == input) return STATUS_PARSE_ERR; // Unparsable in base.
    if(*end != 0) return STATUS_PARSE_ERR; // Leftover garbage.
    if((LONG_MIN == *output || LONG_MAX == *output) && errno == ERANGE) {
        // Input value exceeds range of long.
        return STATUS_RANGE_ERR;
    }
    return STATUS_OK;
}