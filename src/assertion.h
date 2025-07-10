#ifndef __ASSERTION_C
#define __ASSERTION_C
#ifdef __cplusplus
extern "C"{
#endif

#include <math.h>
#include <stdio.h>
#include "string.h"

extern int asserts_failed;
extern int asserts_passed;

#define ASSERT_MACRO(condition, ...) \
if((condition)) \
{ \
    printf("[Failure] %s:%d %s\n", __FILE__, __LINE__, __func__); \
    printf(__VA_ARGS__); \
    asserts_failed++; \
    return; \
} \
else { \
    asserts_passed++; \
}

#define assertStringsEqual(actual, expected) \
ASSERT_MACRO( \
    strCmp((actual), (expected)), \
    "Actual: %s Expected: %s\n", (actual), (expected) \
);

#define assertIntegersEqual(actual, expected) \
ASSERT_MACRO( \
    (actual) != (expected), \
    "\tActual: %ld Expected: %ld\n", (long)(actual), (long)(expected) \
);

#define assertFloatsEqual(actual, expected) \
ASSERT_MACRO( \
    (actual) - (expected) > 0.01 || (actual) - (expected) < -0.01, \
    "\tActual: %f Expected: %f\n", (actual), (expected) \
);

#define assertPointersEqual(actual, expected) \
ASSERT_MACRO( \
    (actual) != (expected), \
    "\tActual: 0x%p Expected: 0x%p\n", (void *)(actual), (void *)(expected) \
);

#define assertTrue(result) \
ASSERT_MACRO( \
    !(result), \
    "\tActual: %s (%d) Expected: %s\n", (result) ? "True" : "False", (result), "True" \
);

#define assertFalse(result) \
ASSERT_MACRO( \
    (result), \
    "\tActual: %s (%d) Expected: %s\n", (result) ? "True" : "False", (result), "False" \
);

#define assertIsNull(result) \
ASSERT_MACRO( \
    (result) != NULL, \
    "\tActual: Not NULL Expected: NULL\n" \
);

#define assertNotNull(result) \
ASSERT_MACRO( \
    (result) == NULL, \
    "\tActual: NULL Expected: Not NULL\n" \
);

#ifdef __cplusplus
}
#endif
#endif
