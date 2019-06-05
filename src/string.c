#include "string.h"

const char ASCII_H_DELIMITERS[] = " \t";
const char ASCII_V_DELIMITERS[] = "\n\r";
char isWhitespace(char character)
{
    const char *delimiter = &(ASCII_H_DELIMITERS[0]);
    while (*delimiter) if (character == *(delimiter++)) return 1;
    return 0;
}

char* afterWhitespace(char* s) {
    while (isWhitespace(*s)) s++;
    return s;
}

char isNumber(char toCheck) {
    return toCheck >= 48 && toCheck <= 57;
}

char* isInteger(const char* toCheck) {
    while (*toCheck) {
        if (!isNumber(*toCheck)) break;
        toCheck++;
    }
    return (char*) toCheck;
}

char* startsWith(const char* toCheck, const char* toMatch) {
    while(*toMatch) {
        if(*toMatch++ != *toCheck++) return NULL;
    }
    return (char*)toCheck;
}

char* endsWith(const char* toCheck, const char* toMatch) {
    if(toCheck == NULL || toMatch == NULL) return 0;
    unsigned int toCheckLength = strlen(toCheck);
    unsigned int toMatchLength = strlen(toMatch);
    const char* toCheckOffset = toCheck + toCheckLength - toMatchLength;
    return startsWith(toCheckOffset, toMatch);
}

char tokenize(
    char** thisToken,
    char** nextDelimiter,
    char** nextToken,
    const char* delimiters) {
    /*Return token, move *nextDelimiter to first delimiter.
    Assumes: thisToken != NULL
    Note:
      *thisToken==*nextDelim implies token equals empty string \0.
      *nextDelimiter can be set to \0 after first call safely.*/
    if (*nextToken == NULL) {
        if (*nextDelimiter) return 0; // Finished processing.
        *nextToken = *thisToken;
    }
    else *thisToken = *nextToken;
    const char* delimiter;
    do {// Foreach character.
        delimiter = delimiters;
        do {// Foreach delimiter, including \0.
            if (**nextToken == *delimiter) {// Delimiter matched.
                *nextDelimiter = *nextToken;
                *nextToken = *delimiter=='\0' ? NULL : *nextToken + 1;
                return 1;
            }
        } while (*(delimiter++));
        (*nextToken)++;
    } while (1);
}

char* stringCopy(const char* string) {
    if (!string) return NULL;
    unsigned int length = strlen(string) + 1;
    char* newString = (char *) malloc(length);
    return newString ? (char *) memcpy(newString, string, length) : NULL;
}

void stringTrimAfterLast(char* string, const char* delimiter) {
    char* lastToken = NULL;
    while (*string) {
        const char* delimiterPointer = delimiter;
        while (*delimiterPointer) {
            if (*string == *delimiterPointer++) lastToken = string;
        }
        string++;
    }
    if (!lastToken || !*lastToken) return;
    *++lastToken = '\0';
}

char* stringJoin(const char* a, const char* b) {
    unsigned int aLen = strlen(a);
    unsigned int bLen = strlen(b);
    unsigned int newStrLen = aLen + bLen;
    char* str = (char *) malloc(newStrLen + 1);
    if (str) {
        memcpy(str, a, aLen);
        memcpy(str+aLen, b, bLen);
        str[newStrLen] = 0;
    }
    return str;
}
