#include "integer.h"
#include "../assertion.h"
#include "../error.h"

void testIntegerParseSuccess() {
    long value;
    int result = longParse("0", 10, &value);
    assertIntegersEqual(result, STATUS_OK);
    assertIntegersEqual(value, 0);

    result = longParse("100", 10, &value);
    assertIntegersEqual(result, STATUS_OK);
    assertIntegersEqual(value, 100);

    result = longParse("-57", 10, &value);
    assertIntegersEqual(result, STATUS_OK);
    assertIntegersEqual(value, -57);

    result = longParse("0xFF", 16, &value);
    assertIntegersEqual(result, STATUS_OK);
    assertIntegersEqual(value, 0xFF);
}

void integerTest() {
    testIntegerParseSuccess();
}