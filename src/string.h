#ifndef __STRING_H
#define __STRING_H
#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern const char ASCII_H_DELIMITERS[];
extern const char ASCII_V_DELIMITERS[];

char isWhitespace(char character);

char* afterWhitespace(char* string);

char isNumber(char toCheck);

char* isInteger(const char* toCheck);

char* startsWith(const char* toCheck, const char* toMatch);

char* endsWith(const char* toCheck, const char* toMatch);

char tokenize(
    char** thisToken,
    char** nextDelimiter,
    char** nextToken,
    const char* delimiters);

char* stringCopy(const char* string);

void stringTrimAfterLast(char* string, const char* delimiter);

char* stringJoin(const char* a, const char* b);

#ifdef __cplusplus
}
#endif
#endif
