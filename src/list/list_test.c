#include <stdio.h>
#include "list.h"
#include "../assertion.h"

struct TestData {
    int value;
};

int testDataCompare(const void *a, const void *b) {
    if(a == b) return 0;
    return 0;
}

void listReleaseNull() {
    listRelease(NULL);
}

void newListHasSizeZero() {
    struct List list;
    listCompose(&list);
    assertIntegersEqual(listSize(&list), 0);

    listRelease(&list);
}

void newListIsEmpty() {
    struct List list;
    listCompose(&list);
    assertTrue(listIsEmpty(&list));

    listRelease(&list);
}

void newListAddFront() {
    struct List list;
    listCompose(&list);
    struct TestData testData;

    listAddHead(&list, &testData);
    assertFalse(listIsEmpty(&list));
    assertIntegersEqual(listSize(&list), 1);

    listRelease(&list);
}

void newListAddRear() {
    struct List list;
    listCompose(&list);
    struct TestData testData;

    listAddTail(&list, &testData);
    assertFalse(listIsEmpty(&list));
    assertIntegersEqual(listSize(&list), 1);

    listRelease(&list);
}

void listSizeOneRemoveFront() {
    struct List list;
    listCompose(&list);
    struct TestData testData;
    listAddHead(&list, &testData);
 
    void *data = listRemoveHead(&list);
    assertPointersEqual(data, &testData);
    assertTrue(listIsEmpty(&list));
    assertIntegersEqual(listSize(&list), 0);

    listRelease(&list);
}

void listSizeOneRemoveRear() {
    struct List list;
    listCompose(&list);
    struct TestData testData;
    listAddTail(&list, &testData);

    void *data = listRemoveTail(&list);
    assertPointersEqual(data, &testData);
    assertTrue(listIsEmpty(&list));
    assertIntegersEqual(listSize(&list), 0);

    listRelease(&list);
}

void listSizeTwoRemoveFront() {
    struct List list;
    listCompose(&list);
    struct TestData testData1;
    struct TestData testData2;
    listAddTail(&list, &testData1);
    listAddTail(&list, &testData2);

    void *data = listRemoveHead(&list);
    assertPointersEqual(data, &testData1);
    assertFalse(listIsEmpty(&list));
    assertIntegersEqual(listSize(&list), 1);

    listRelease(&list);
}

void listSizeTwoRemoveRear() {
    struct List list;
    listCompose(&list);
    struct TestData testData1;
    struct TestData testData2;
    listAddTail(&list, &testData1);
    listAddTail(&list, &testData2);

    void *data = listRemoveTail(&list);
    assertPointersEqual(data, &testData2);
    assertFalse(listIsEmpty(&list));
    assertIntegersEqual(listSize(&list), 1);

    listRelease(&list);
}

void newListDoesNotContain() {
    struct List list;
    listCompose(&list);
    struct TestData testData;

    int result = listContains(&list, &testData);
    assertFalse(result);

    listRelease(&list);
}

void listSizeOneDoesContain() {
    struct List list;
    listCompose(&list);
    list.compare = &testDataCompare;
    struct TestData testData;
    listAddTail(&list, &testData);

    int result = listContains(&list, &testData);
    assertTrue(result);

    listRelease(&list);
}

void listSizeTwoDoesContain() {
    struct List list;
    listCompose(&list);
    list.compare = &testDataCompare;
    struct TestData testData1;
    struct TestData testData2;
    listAddTail(&list, &testData1);
    listAddTail(&list, &testData2);

    int result = listContains(&list, &testData2);
    assertTrue(result);

    listRelease(&list);
}

void iteratorNewList() {
    struct List list;
    listCompose(&list);
    list.compare = &testDataCompare;

    struct Iterator iterator = listIterator(&list);
    assertIsNull(listNext(&iterator));

    listRelease(&list);
}

void iteratorListSizeOne() {
    struct List list;
    listCompose(&list);
    list.compare = &testDataCompare;
    struct TestData testData;
    listAddTail(&list, &testData);
    struct Iterator iterator = listIterator(&list);

    assertPointersEqual(listNext(&iterator), &testData);
    assertIsNull(listNext(&iterator));

    listRelease(&list);
}

void iteratorListSizeTwo() {
    struct List list;
    listCompose(&list);
    list.compare = &testDataCompare;
    struct TestData testData1;
    struct TestData testData2;
    listAddTail(&list, &testData1);
    listAddTail(&list, &testData2);
    struct Iterator iterator = listIterator(&list);

    assertPointersEqual(listNext(&iterator), &testData1);
    assertPointersEqual(listNext(&iterator), &testData2);
    assertIsNull(listNext(&iterator));

    listRelease(&list);
}

void testListAddCurrentForward() {
    struct List list;
    listCompose(&list);
    struct TestData testData1;
    struct TestData testData2;
    struct Iterator iterator = listIterator(&list);
    listAddCurrent(&iterator, &testData1);
    listAddCurrent(&iterator, &testData2);
    assertIntegersEqual(listSize(&list), 2);

    iterator = listIterator(&list);
    assertPointersEqual(listNext(&iterator), &testData1);
    assertPointersEqual(listNext(&iterator), &testData2);
    assertIsNull(listNext(&iterator));
}

void testListAddCurrentReverse() {
    struct List list;
    listCompose(&list);
    struct TestData testData1;
    struct TestData testData2;
    struct Iterator iterator = listIteratorMode(&list, ITERATION_REVERSE);
    listAddCurrent(&iterator, &testData1);
    listAddCurrent(&iterator, &testData2);
    assertIntegersEqual(listSize(&list), 2);

    iterator = listIterator(&list);
    assertPointersEqual(listNext(&iterator), &testData2);
    assertPointersEqual(listNext(&iterator), &testData1);
    assertIsNull(listNext(&iterator));
}

void listTest() {
    listReleaseNull();
    newListHasSizeZero();
    newListIsEmpty();
    newListAddFront();
    newListAddRear();
    listSizeOneRemoveFront();
    listSizeOneRemoveRear();
    listSizeTwoRemoveFront();
    listSizeTwoRemoveRear();
    newListDoesNotContain();
    listSizeOneDoesContain();
    listSizeTwoDoesContain();
    iteratorNewList();
    iteratorListSizeOne();
    iteratorListSizeTwo();
    testListAddCurrentForward();
    testListAddCurrentReverse();
}
