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

/*TODO: Name these better, or change their interface.*/
char* afterWhitespace(char* string);
char* afterInteger(char *toCheck);
char* afterNumber(char *toCheck);
char* afterQuotedString(char* toCheck);
char* afterLineBreak(char* toCheck);
char* afterDigits(const char* toCheck);

unsigned int strCpyTo(char *target, const char *value);
unsigned int strCpyNTo(char *target, unsigned int length, const char *value);

unsigned long long strHash(const void* ptr);
char* strCopy(const char* string);
char* strCopyN(const char* string, unsigned int length);
char* strJoin(const char* a, const char* b);
char strCmp(const char* toCheck, const char* toMatch);
char strCmpN(const char* toCheck, const char* toMatch, unsigned int size);
int safeStrCmp(const char *a, const char* b);
char* strStartsWith(const char* toCheck, const char* toMatch);
char* strEndsWith(const char* toCheck, const char* toMatch);
void strTrimAfterLast(char* string, const char* delimiter);
const char *strFindFirst(const char *str, char value);
const char *strFindLast(const char* str, char value);

char tokenize(
    char** thisToken,
    char** nextDelimiter,
    char** nextToken,
    const char* delimiters);

void strReplaceChar(char *input, char toReplace, char value);
char *strReplace(
    char *input,
    unsigned int offset,
    unsigned int toRemove,
    const char *toInsert);

#ifdef __cplusplus
}
#endif
#endif
