#include "unicode.h"

ENCODING detect_encoding(unsigned char *buffer, unsigned int size) {
    if (size >= 4) {
        if (
            (buffer[0] == 0 && buffer[1] == 0 && buffer[2] == 0xFE && buffer[3] == 0xFF) ||
            (buffer[0] == 0 && buffer[1] == 0 && buffer[2] == 0)
        ) return UTF32BE;
        if (
            (buffer[0] == 0xFF && buffer[1] == 0xFE && buffer[2] == 0 && buffer[3] == 0) ||
            (buffer[1] == 0 && buffer[2] == 0 && buffer[3] == 0)
        ) return UTF32LE;
    }
    if (size >= 2) {
        if (
            (buffer[0] == 0xFE && buffer[1] == 0xFF) ||
            buffer[0] == 0
        ) return UTF16BE;
        if (
            (buffer[0] == 0xFF && buffer[1] == 0xFE) ||
            buffer[1] == 0
        ) return UTF16LE;
    }
    if (size >= 3) {
        if (buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF) return UTF8;
    }
    return UTF8;
}
