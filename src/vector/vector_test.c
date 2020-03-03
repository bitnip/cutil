#include <stdio.h>
#include "vector.h"
#include "../assertion.h"

void vectorReleaseNull() {
    vectorRelease(NULL);
}

void newVectorHasSizeZero() {
    struct Vector vector;

    vectorCompose(&vector);
    assertIntegersEqual(vectorSize(&vector), 0);

    vectorRelease(&vector);
}

void newVectorIsEmpty() {
    struct Vector vector;

    vectorCompose(&vector);
    assertTrue(vectorIsEmpty(&vector));

    vectorRelease(&vector);
}

void emptyVectorAdd() {
    struct Vector vector;
    char value[] = "hello world";
    vectorCompose(&vector);
    vectorAddTail(&vector, &value);
    assertIntegersEqual(vectorSize(&vector), 1);
    vectorRelease(&vector);
}

void emptyVectorGet() {
    struct Vector vector;
    vectorCompose(&vector);
    char* value = vectorGet(&vector, 0);
    assertPointersEqual(value, NULL);
    assertIntegersEqual(vectorSize(&vector), 0);
    vectorRelease(&vector);
}

void emptyVectorAddGet() {
    struct Vector vector;
    char value[] = "hello";
    vectorCompose(&vector);
    vectorAddTail(&vector, value);
    char* result = vectorGet(&vector, 0);
    assertPointersEqual(result, value);
    vectorRelease(&vector);
}

void iteratorNewVector() {
    struct Vector vector;
    vectorCompose(&vector);

    struct Iterator iterator = vectorIterator(&vector);
    assertIsNull(vectorNext(&iterator));

    vectorRelease(&vector);
}

void iteratorVectorSizeOne() {
    struct Vector vector;
    vectorCompose(&vector);
    char value[] = "hello world";
    vectorAddTail(&vector, &value);
    struct Iterator iterator = vectorIterator(&vector);

    assertPointersEqual(vectorNext(&iterator), &value);
    assertIsNull(vectorNext(&iterator));

    vectorRelease(&vector);
}

void iteratorVectorSizeTwo() {
    struct Vector vector;
    vectorCompose(&vector);
    char value1[] = "hello world";
    char value2[] = "foo bar";
    vectorAddTail(&vector, &value1);
    vectorAddTail(&vector, &value2);
    struct Iterator iterator = vectorIterator(&vector);

    assertPointersEqual(vectorNext(&iterator), &value1);
    assertPointersEqual(vectorNext(&iterator), &value2);
    assertIsNull(vectorNext(&iterator));

    vectorRelease(&vector);
}

void vectorTest() {
    vectorReleaseNull();
    newVectorHasSizeZero();
    newVectorIsEmpty();
    emptyVectorAdd();
    emptyVectorGet();
    emptyVectorAddGet();
    iteratorNewVector();
    iteratorVectorSizeOne();
    iteratorVectorSizeTwo();
}