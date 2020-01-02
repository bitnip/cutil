#include "string.h"

const char ASCII_H_DELIMITERS[] = " \t";
const char ASCII_V_DELIMITERS[] = "\n\r";

char isWhitespaceChar(char character) {
    const char *delimiter = &(ASCII_H_DELIMITERS[0]);
    while (*delimiter) if (character == *(delimiter++)) return 1;
    return 0;
}

char* afterWhitespace(char* s) {
    while (isWhitespaceChar(*s)) s++;
    return s;
}

char isDigitChar(char toCheck) {
    return toCheck >= 48 && toCheck <= 57;
}

char* afterDigits(const char* toCheck) {
    while(*toCheck) {
        if(!isDigitChar(*toCheck)) break;
        toCheck++;
    }
    return (char*) toCheck;
}

char* afterInteger(char *toCheck) {
    char *offset = toCheck;
    if(*toCheck == '-' || *toCheck == '+') {
        // Possibly number starting with sign.
        offset = afterDigits(1+toCheck);
        if(offset == 1+toCheck) {
            // No number after sign.
            return toCheck;
        }
    } else {
        // Possibly number starting with integer.
        offset = afterDigits(offset);
        if(offset == toCheck) {
            // No integer portion.
            return toCheck;
        }
    }
    return offset;
}

char* afterNumber(char* toCheck) {
    char* offset = afterInteger(toCheck);
    if(offset==toCheck) return toCheck;
    if(*offset == ',' || *offset == '.') {
        // Possibly decimal section.
        toCheck = afterDigits(1+offset);
        if(toCheck != 1+offset) {
            offset = toCheck;
        }
    }
    if(*offset == 'e' || *offset == 'E') {
        toCheck = afterInteger(1+offset);
        if(toCheck != 1+offset) {
            offset = toCheck;
        }
    }
    return offset;
}

char *afterQuotedString(char *toCheck) {
    if(*toCheck != '"') return toCheck;
    char *offset = 1+toCheck;
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

char* afterLineBreak(char* toCheck) {
    if(*toCheck == '\r') toCheck++;
    if(*toCheck == '\n') toCheck++;
    return toCheck;
}

char strCmpN(const char* toCheck, const char* toMatch, unsigned int size) {
    while(size--) {
        char result = *toMatch++ - *toCheck++;
        if(result) return result;
    }
    return 0;
}

char strCmp(const char* toCheck, const char* toMatch) {
    do {
        char result = *toMatch - *toCheck++;
        if(result) return result;
    } while(*(toMatch++));
    return 0;
}

int safeStrCmp(const char *a, const char* b) {
    if(a == b) return 0;
    else if(a == NULL) return -1;
    else if(b == NULL) return 1;
    else return strCmp(a, b);
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
    char** thisToken,
    char** nextDelimiter,
    char** nextToken,
    const char* delimiters) {
    //TODO: beter doc. Maybe `Returns where the current token terminates
    /*Return true if *nextToken , move *nextDelimiter to first delimiter.
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

unsigned int strCpyTo(char *target, const char *value) {
    unsigned int byteCount = 0;
    while(*value) {
        *(target++) = *(value++);
        byteCount++;
    }
    return byteCount;
}

unsigned int strCpyNTo(char *target, unsigned int length, const char *value) {
    unsigned int byteCount = 0;
    while(*value && length--) {
        *(target++) = *(value++);
        byteCount++;
    }
    return byteCount;
}

char* strCopy(const char* string) {
    if (!string) return NULL;
    unsigned int length = strlen(string);
    char *newString = (char *) malloc(length + 1);
    newString[length] = 0;
    if(!newString) return NULL;

    strCpyTo(newString, string);
    return newString;
}

char* strCopyN(const char* string, unsigned int length) {
    /*Returns a string of length+1 bytes, copies string until length or \0.*/
    char* result = (char *)malloc(length + 1);
    char* ptr = result;
    while(length-- && *string) {
        *(ptr++) = *(string++);
    }
    *ptr = 0;
    return result;
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

char* strJoin(const char* a, const char* b) {
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

const char *strFindFirst(const char *str, char value) {
    while(*str) {
        if(*str==value) return str;
        str++;
    }
    return NULL;
}

const char *strFindLast(const char *str, char value) {
    const char *begining = str;
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

void strReplaceChar(char *input, char toReplace, char value) {
    while(*input) {
        if(*input == toReplace) {
            *input = value;
        }
        input++;
    }
}

char *strReplace(
        char *input,
        unsigned int offset,
        unsigned int toRemove,
        const char *toInsert) {
    unsigned int inputLength = strlen(input);

    if(offset+toRemove > inputLength) return NULL;

    unsigned int insertLength = strlen(toInsert);
    unsigned int resultLength = inputLength+insertLength-toRemove;
    char *result = (char *)malloc(sizeof(char)*(resultLength+1));
    result[resultLength] = 0;
    memcpy(result, input, offset);
    memcpy(result+offset, toInsert, insertLength);
    memcpy(result+offset+insertLength, input+offset+toRemove, resultLength-offset-insertLength);
    return result;
}