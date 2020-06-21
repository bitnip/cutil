#include "string.h"
#include "assertion.h"

void oneCharacterInteger() {
    char input[] = "1";
    char* offset = afterInteger(input);
    assertTrue(input + 1 == offset);
}

void multiCharacterInteger() {
    char input[] = "255";
    char* offset = afterInteger(input);
    assertTrue(input + 3 == offset);
}

void positiveInteger() {
    char input[] = "+123";
    char* offset = afterInteger(input);
    assertTrue(input + 4 == offset);
}

void negativeInteger() {
    char input[] = "-456";
    char* offset = afterInteger(input);
    assertTrue(input + 4 == offset);
}

void emptyStringNotInteger() {
    char input[] = "\0";
    char* offset = afterInteger(input);
    assertTrue(input == offset);
}

void letterNotInteger() {
    char input[] = "a";
    char* offset = afterInteger(input);
    assertTrue(input == offset);
}

void mixedIntegerAndLetter() {
    char input[] = "1a";
    char* offset = afterInteger(input);
    assertTrue(input + 1 == offset);
}

void integerIsNumber() {
    char input[] = "789";
    char *offset = afterNumber(input);
    assertTrue(input + 3 == offset);
}

void decimalIsNumber() {
    char input[] = "1.0";
    char *offset = afterNumber(input);
    assertTrue(input + 3 == offset);
}

void exponantIsNumber() {
    char input[] = "1.0E10";
    char *offset = afterNumber(input);
    assertTrue(input + 6 == offset);
}

void positiveExponantIsNumber() {
    char input[] = "+1.0E+10";
    char *offset = afterNumber(input);
    assertTrue(input + 8 == offset);
}

void negativeExponantIsNumber() {
    char input[] = "-1.0E-10";
    char *offset = afterNumber(input);
    assertTrue(input + 8 == offset);
}

void emptyStringNotQuotedString() {
    char input[] = "";
    char *offset = afterQuotedString(input);
    assertTrue(input == offset);
}

void emptyQuotesAreQuotedString() {
    char input[] = "\"\"";
    char *offset = afterQuotedString(input);
    assertTrue(input + 2 == offset);
}

void quotedSingleCharacterIsQuotedString() {
    char input[] = "\"a\"";
    char *offset = afterQuotedString(input);
    assertTrue(input + 3 == offset);
}

void quotedEscapedQuoteIsQuotedString() {
    char input[] = "\"\\\"\"";
    char *offset = afterQuotedString(input);
    assertTrue(input + 4 == offset);
}

void cmpEmptyStrings() {
    char input[] = "";
    char result = strCmp(input, input);
    assertIntegersEqual(result, 0);
}

void cmpDifferentStrings() {
    char input1[] = "a";
    char input2[] = "b";
    char result = strCmp(input1, input2);
    assertIntegersEqual(result, 1);
}

void cmpDifferentLengthStrings() {
    char input1[] = "aa";
    char input2[] = "ab";
    char result = strCmp(input1, input2);
    assertIntegersEqual(result, 1);
}

void multiCharacterMatch() {
    char* result = strStartsWith("start", "st");
    assertNotNull(result);
}

void singleCharacterMatch() {
    char* result = strStartsWith("start", "s");
    assertNotNull(result);
}

void startsWithIdenticalInputIsTrue() {
    char* result = strStartsWith("asdf", "asdf");
    assertNotNull(result);
}

void emptyStringsMatch() {
    char* result = strStartsWith("", "");
    assertNotNull(result);
}

void emptyStringDoesntStartWithCharacter() {
    char* result = strStartsWith("", "a");
    assertIsNull(result);
}

void multiCharacterDontMatch() {
    char* result = strStartsWith("asdf", "ab");
    assertIsNull(result);
}

void fullMatchMustExist() {
    char* result = strStartsWith("as", "asdf");
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

void testStrReplace() {
    char ello[] = "ello"; 
    char *result = strReplace(ello, 0, 0, "h");
    assertStringsEqual(result, "hello");
    free(result);

    char *wonderful = "wonderful";
    result = strReplace(wonderful, 2, 4, "e");
    assertStringsEqual(result, "woeful");
    free(result);

    char *empty = "";
    result = strReplace(empty, 0, 0, "x");
    assertStringsEqual(result, "x");
    free(result);
}

void testStrCmpToDelim() {
    char *empty = "";
    char *asdf = "asdf";
    char *asdfg = "asdfg";

    char result = strCmpToDelim(empty, empty, empty+1);
    assertIntegersEqual(result, 0);

    result = strCmpToDelim(asdf, asdf, asdf+4);
    assertIntegersEqual(result, 0);

    result = strCmpToDelim(asdf, empty, empty+1);
    assertIntegersEqual(result, -'a');

    result = strCmpToDelim(empty, asdf, asdf+4);
    assertIntegersEqual(result, 'a');

    result = strCmpToDelim(asdfg, asdf, asdf+4);
    assertIntegersEqual(result, -'g');

    result = strCmpToDelim(asdf, asdfg, asdfg+5);
    assertIntegersEqual(result, 'g');
}

void testStrCmp() {
    unsigned int result = strCmp("Apple", "A");
    assertIntegersEqual(result, -'p');
}

void testStrCopy() {
    char *result = strCopy(NULL);
    assertPointersEqual(result, NULL);
}

void stringTest() {
    oneCharacterInteger();
    multiCharacterInteger();
    letterNotInteger();
    mixedIntegerAndLetter();
    emptyStringNotInteger();
    positiveInteger();
    negativeInteger();
    integerIsNumber();
    decimalIsNumber();
    exponantIsNumber();
    positiveExponantIsNumber();
    negativeExponantIsNumber();
    emptyStringNotQuotedString();
    emptyQuotesAreQuotedString();
    quotedSingleCharacterIsQuotedString();
    quotedEscapedQuoteIsQuotedString();
    cmpEmptyStrings();
    cmpDifferentStrings();
    cmpDifferentLengthStrings();

    multiCharacterMatch();
    singleCharacterMatch();
    emptyStringsMatch();
    emptyStringDoesntStartWithCharacter();
    multiCharacterDontMatch();
    fullMatchMustExist();
    startsWithIdenticalInputIsTrue();
    testStrCmpToDelim();
    testStrCmp();
    testStrCopy();

    testTokenize();
    testTokenize2();
    testStrReplace();
}
