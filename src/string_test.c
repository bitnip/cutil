#include "string.h"
#include "assertion.h"

void oneCharacterInteger() {
    char input[] = "1";
    char* offset = isInteger(input);
    assertTrue(input + 1 == offset);
}

void multiCharacterInteger() {
    char input[] = "255";
    char* offset = isInteger(input);
    assertTrue(input + 3 == offset);
}

void emptyStringNotInteger() {
    char input[] = "\0";
    char* offset = isInteger(input);
    assertTrue(input == offset);
}

void letterNotInteger() {
    char input[] = "a";
    char* offset = isInteger(input);
    assertTrue(input == offset);
}

void mixedIntegerAndLetter() {
    char input[] = "1a";
    char* offset = isInteger(input);
    assertTrue(input + 1 == offset);
}

void multiCharacterMatch() {
    char* result = startsWith("start", "st");
    assertNotNull(result);
}

void singleCharacterMatch() {
    char* result = startsWith("start", "s");
    assertNotNull(result);
}

void startsWithIdenticalInputIsTrue() {
    char* result = startsWith("asdf", "asdf");
    assertNotNull(result);
}

void emptyStringsMatch() {
    char* result = startsWith("", "");
    assertNotNull(result);
}

void emptyStringDoesntStartWithCharacter() {
    char* result = startsWith("", "a");
    assertIsNull(result);
}

void multiCharacterDontMatch() {
    char* result = startsWith("asdf", "ab");
    assertIsNull(result);
}

void fullMatchMustExist() {
    char* result = startsWith("as", "asdf");
    assertIsNull(result);
}

void testTokenize2() {
    char input[] = "";
    char *thisToken = &input[0];
    char *nextDelim = NULL;
    char *nextToken = NULL;
    char count = 0;
    while (tokenize(&thisToken, &nextDelim, &nextToken,  "/")) {
        count++;
        assertTrue(count<2);
    }
    assertIntegersEqual(count, 1);

    count=0;
    nextDelim=NULL;
    nextToken=NULL;
    char input2[] = "/";
    thisToken = &input2[0];
    while (tokenize(&thisToken, &nextDelim, &nextToken, "/")) {
        count++;
        assertTrue(count<3);
    }
    assertIntegersEqual(count, 2);

    count=0;
    nextDelim=NULL;
    nextToken=NULL;
    char input3[] = "//";
    thisToken = &input3[0];
    while (tokenize(&thisToken, &nextDelim, &nextToken, "/")) {
        count++;
        assertTrue(count<4);
    }
    assertIntegersEqual(count, 3);
}

void testTokenize() {
    char input[] = "//a/b/c/d/e/f/g/h/i/j/k/l/m/n/o/p/q/r/s/t/u/v/w/x/y/z//";
    char *thisToken = &input[0];
    char *nextDelim = NULL;
    char *nextToken = NULL;
    char *tInput = thisToken;

    char hasToken;
    // First 2 tokens are one apart.
    for (char i=0; i<2; i++) {
        hasToken = tokenize(&thisToken, &nextDelim, &nextToken,  "/");
        assertTrue(hasToken);
        assertStringsEqual(thisToken, tInput);
        tInput++;
    }
    // Next 26 tokens are 2 apart.
    for (char i=0; i<26; i++) {
        hasToken = tokenize(&thisToken, &nextDelim, &nextToken,  "/");
        assertTrue(hasToken);
        assertStringsEqual(thisToken, tInput);
        tInput += 2;
    }
    // Last 2 tokens are one apart.
    for (char i=0; i<2; i++) {
        hasToken = tokenize(&thisToken, &nextDelim, &nextToken,  "/");
        assertTrue(hasToken);
        assertStringsEqual(thisToken, tInput);
        tInput++;
    }
    hasToken = tokenize(&thisToken, &nextDelim, &nextToken,  "/");
    assertFalse(hasToken);
}

void stringTest() {
    oneCharacterInteger();
    multiCharacterInteger();
    letterNotInteger();
    mixedIntegerAndLetter();
    emptyStringNotInteger();

    multiCharacterMatch();
    singleCharacterMatch();
    emptyStringsMatch();
    emptyStringDoesntStartWithCharacter();
    multiCharacterDontMatch();
    fullMatchMustExist();
    startsWithIdenticalInputIsTrue();

    testTokenize();
    testTokenize2();
}
