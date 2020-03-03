#ifndef __UNICODE_H
#define __UNICODE_H
#ifdef __cplusplus
extern "C"{
#endif

enum UNICODE_ENCODING {
    UNICODE_UTF32BE,
    UNICODE_UTF32LE,
    UNICODE_UTF16BE,
    UNICODE_UTF16LE,
    UNICODE_UTF8
}

ENCODING detect_encoding(unsigned char* buffer, unsigned int size);


#ifdef __cplusplus
}
#endif
#endif
