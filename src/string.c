#include <stdlib.h>
#include "string.h"

const char ASCII_H_DELIMITERS[] = " \t";
const char ASCII_V_DELIMITERS[] = "\r\n";

static inline char isWhitespaceChar(char character) {
    const char* delimiter = &(ASCII_H_DELIMITERS[0]);
    while (*delimiter) if (character == *(delimiter++)) return 1;
    return 0;
}

const char* strAfterWhitespace(const char* s) {
    while (isWhitespaceChar(*s)) s++;
    return s;
}

static inline char isDigitChar(char toCheck) {
    return toCheck >= 48 && toCheck <= 57;
}

char* strAfterDigits(const char* toCheck) {
    while(*toCheck) {
        if(!isDigitChar(*toCheck)) break;
        toCheck++;
    }
    return (char*) toCheck;
}

const char* strAfterInteger(const char* toCheck) {
    const char* offset = toCheck;
    if(*toCheck == '-' || *toCheck == '+') {
        // Possibly number starting with sign.
        offset = strAfterDigits(1+toCheck);
        if(offset == 1+toCheck) {
            // No number after sign.
            return toCheck;
        }
    } else {
        // Possibly number starting with integer.
        offset = strAfterDigits(offset);
        if(offset == toCheck) {
            // No integer portion.
            return toCheck;
        }
    }
    return offset;
}

const char* strAfterNumber(const char* toCheck) {
    const char* offset = strAfterInteger(toCheck);
    if(offset==toCheck) return toCheck;
    if(*offset == ',' || *offset == '.') {
        // Possibly decimal section.
        toCheck = strAfterDigits(1+offset);
        if(toCheck != 1+offset) {
            offset = toCheck;
        }
    }
    if(*offset == 'e' || *offset == 'E') {
        toCheck = strAfterInteger(1+offset);
        if(toCheck != 1+offset) {
            offset = toCheck;
        }
    }
    return offset;
}

char* strAfterQuotedString(char* toCheck) {
    if(*toCheck != '"') return toCheck;
    char* offset = 1+toCheck;
    char escaped = 0;
    while(*offset) {
        if(*offset == '\\') {
            escaped = 1;
            offset++;
            continue;
        }
        if(!escaped && *offset == '"') {
            return 1 + offset;
        }
        escaped = 0;
        offset++;
    }
    return toCheck;
}

char* strAfterLineBreak(char* toCheck) {
    if(*toCheck == '\r') toCheck++;
    if(*toCheck == '\n') toCheck++;
    return toCheck;
}

char strCmpToDelim(
        const char* toCheck,
        const char* toMatchStart,
        const char* toMatchDelim) {
    do {
        if(toMatchStart >= toMatchDelim) {
            return -*toCheck;
        }
        char result = *toMatchStart++ - *toCheck;
        if(result) return result;
    } while(*toCheck++);
    return 0;
}

char strCmpN(const char* toCheck, const char* toMatch, unsigned int size) {
    /*
    Note: `toCheck` and `toMatch` cannot be NULL.
    Note: Assumes `toCheck` and `toMatch` are null terminated.
    */
    while(size--) {
        char result = *toMatch++ - *toCheck++;
        if(result) return result;
    }
    return 0;
}

char strCmp(const char* toCheck, const char* toMatch) {
    /*Note: Assumes `toCheck` and `toMatch` are null terminated.*/
    if(toCheck == toMatch) return 0;
    else if(toCheck == NULL) return -1;
    else if(toMatch  == NULL) return 1;
    do {
        char result = *toMatch - *toCheck++;
        if(result) return result;
    } while(*(toMatch++));
    return 0;
}

char* strStartsWith(const char* toCheck, const char* toMatch) {
    while(*toMatch) {
        if(*toMatch++ != *toCheck++) return NULL;
    }
    return (char*)toCheck;
}

char* strEndsWith(const char* toCheck, const char* toMatch) {
    if(toCheck == NULL || toMatch == NULL) return 0;
    unsigned int toCheckLength = strlen(toCheck);
    unsigned int toMatchLength = strlen(toMatch);
    const char* toCheckOffset = toCheck + toCheckLength - toMatchLength;
    return strStartsWith(toCheckOffset, toMatch);
}

char tokenize(
    const char** thisToken,
    const char** nextDelimiter,
    const char** nextToken,
    const char* delimiters) {
    /*
    Subsequent calls advance thisToken past next delimiter.
    Note:
      thisToken is advanced to the start of the next token.
      nextDelimiter is set to the end of thisToken.
      nextToken is set to the following token or NULL.
    */
    if (*nextToken == NULL) {
        if (*nextDelimiter) return 0; // Finished processing.
        *nextToken = *thisToken;
    }
    else *thisToken = *nextToken;
    const char* delimiter;
    do {// Foreach character.
        delimiter = delimiters;
        do {// Foreach delimiter, including \0.
            if (**nextToken == *delimiter) { // Delimiter matched.
                *nextDelimiter = *nextToken;
                *nextToken = *delimiter=='\0' ? NULL : *nextToken + 1;
                return 1;
            }
        } while (*(delimiter++));
        (*nextToken)++;
    } while (1);
}

unsigned int strCpyTo(char* target, const char* value) {
    unsigned int byteCount = 0;
    while(*value) {
        *(target++) = *(value++);
        byteCount++;
    }
    return byteCount;
}

unsigned int strCpyNTo(char* target, unsigned int length, const char* value) {
    unsigned int byteCount = 0;
    while(*value && length--) {
        *(target++) = *(value++);
        byteCount++;
    }
    return byteCount;
}

char* strCopy(const char* string) {
    if(!string) return NULL;
    unsigned int length = strlen(string);
    char* newString = (char*)malloc(length + 1);
    newString[length] = 0;
    if(!newString) return NULL;

    strCpyTo(newString, string);
    return newString;
}

char* strCopyN(const char* string, unsigned int length) {
    /*Returns a string of length+1 bytes, copies string until length or \0.*/
    char* result = (char*)malloc(length + 1);
    char* ptr = result;
    while(length-- && *string) {
        *(ptr++) = *(string++);
    }
    *ptr = 0;
    return result;
}

char* strJoin(const char* a, const char* b) {
    unsigned int aLen = strlen(a);
    unsigned int bLen = strlen(b);
    unsigned int newStrLen = aLen + bLen;
    char* str = (char*) malloc(newStrLen + 1);
    if (str) {
        memcpy(str, a, aLen);
        memcpy(str+aLen, b, bLen);
        str[newStrLen] = 0;
    }
    return str;
}

const char* strFindFirst(const char* str, char value) {
    while(*str) {
        if(*str==value) return str;
        str++;
    }
    return NULL;
}

const char* strFindLast(const char* str, char value) {
    const char* begining = str;
    while(*str) str++;
    while(str >= begining) {
        if(*str==value) return str;
        str--;
    }
    return NULL;
}

unsigned long long strHash(const void* string) {
    const int p = 31;
    const int m = 1e9+9;
    unsigned long long hash = 0;
    unsigned long long pPow = 1;
    char c;
    char* ptr = (char*)string;
    while((c = *ptr++)) {
        hash = (hash + (c - 'a' + 1) * pPow) % m;
        pPow = (pPow * p) % m;
    }
    return hash;
}

char* strReplace(
        char* input,
        unsigned int offset,
        unsigned int toRemove,
        const char* toInsert) {
    unsigned int inputLength = strlen(input);

    if(offset+toRemove > inputLength) {
        if(toInsert) {
            return strCopy(toInsert);
        } else {
            char* result = malloc(sizeof(char));
            result[0] = 0;
            return result;
        }
    }

    unsigned int insertLength = strlen(toInsert);
    unsigned int resultLength = inputLength+insertLength-toRemove;
    char* result = (char*)malloc(sizeof(char)*(resultLength+1));
    result[resultLength] = 0;
    memcpy(result, input, offset);
    memcpy(result+offset, toInsert, insertLength);
    memcpy(result+offset+insertLength, input+offset+toRemove, resultLength-offset-insertLength);
    return result;
}