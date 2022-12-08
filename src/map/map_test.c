#include <stdio.h>
#include "map.h"
#include "../assertion.h"
#include "../string.h"

void mapReleaseNull() {
    mapRelease(NULL);
}

void newMapHasSizeZero() {
    struct Map map;

    mapCompose(&map);
    assertIntegersEqual(mapSize(&map), 0);

    mapRelease(&map);
}

void emptyMapAdd() {
    struct Map map;
    char key[] = "hello";
    char value[] = "world";
    mapCompose(&map);
    map.hashKey = (long long unsigned int (*)(const void *))strHash;
    mapAdd(&map, key, value);
    assertIntegersEqual(mapSize(&map), 1);
    mapRelease(&map);
}

void emptyMapGet() {
    struct Map map;
    char key[] = "hello";
    mapCompose(&map);
    map.hashKey = (long long unsigned int (*)(const void *))strHash;
    char* value = mapGet(&map, key);
    assertPointersEqual(value, NULL);
    assertIntegersEqual(mapSize(&map), 0);
    mapRelease(&map);
}

void emptyMapAddGet() {
    struct Map map;
    char key[] = "hello";
    char value[] = "world";
    mapCompose(&map);
    map.hashKey = (long long unsigned int (*)(const void *))strHash;
    mapAdd(&map, key, value);
    char* result = mapGet(&map, key);
    assertPointersEqual(result, value);
    mapRelease(&map);
}

void emptyMapAddAddGet() {
    struct Map map;
    char key0[] = "hello";
    char value0[] = "world";
    char key1[] = "foo";
    char value1[] = "bar";
    // Setup `map`.
    mapCompose(&map);
    map.hashKey = (long long unsigned int (*)(const void *))strHash;
    // Add "hello": "world" to `map`.
    mapAdd(&map, key0, value0);
    // Add "foo": "bar" to `map`.
    mapAdd(&map, key1, value1);
    // Verify "hello" returns "world".
    char* result = mapGet(&map, key0);
    assertPointersEqual(result, value0);
    // Verify "foo" returns "bar".
    result = mapGet(&map, key1);
    assertPointersEqual(result, value1);
    // Release map.
    mapRelease(&map);
}

void emptyMapAddGetMiss() {
    struct Map map;
    char key0[] = "hello";
    char value0[] = "world";
    char key1[] = "foo";
    // Setup `map`.
    mapCompose(&map);
    map.hashKey = (long long unsigned int (*)(const void *))strHash;
    // Add "hello": "world" to `map`.
    mapAdd(&map, key0, value0);
    // Verify the key "foo" is not in `map`.
    char* value = mapGet(&map, key1);
    assertPointersEqual(value, NULL);
    // Release map.
    mapRelease(&map);
}

void iteratorNewMap() {
    struct Map map;
    mapCompose(&map);

    struct Iterator iterator = mapIterator(&map);
    assertIsNull(mapNext(&iterator));

    mapRelease(&map);
}

void iteratorMapSizeOne() {
    struct Map map;
    mapCompose(&map);
    map.hashKey = (long long unsigned int (*)(const void *))strHash;
    char key[] = "hello";
    char value[] = "world";
    mapAdd(&map, key, value);
    struct Iterator iterator = mapIterator(&map);

    assertPointersEqual(mapNext(&iterator), &value);
    assertIsNull(mapNext(&iterator));

    mapRelease(&map);
}

void iteratorMapSizeTwo() {
    struct Map map;
    mapCompose(&map);
    map.hashKey = (long long unsigned int (*)(const void *))strHash;
    char key0[] = "hello";
    char value0[] = "world";
    char key1[] = "foo";
    char value1[] = "bar";
    mapAdd(&map, &key0, &value0);
    mapAdd(&map, &key1, &value1);
    struct Iterator iterator = mapIterator(&map);

    assertPointersEqual(mapNext(&iterator), &value0);
    assertPointersEqual(mapNext(&iterator), &value1);
    assertIsNull(mapNext(&iterator));

    mapRelease(&map);
}

void mapTest() {
    mapReleaseNull();
    newMapHasSizeZero();
    emptyMapAdd();
    emptyMapGet();
    emptyMapAddAddGet();
    emptyMapAddGet();
    emptyMapAddGetMiss();
    iteratorNewMap();
    iteratorMapSizeOne();
    iteratorMapSizeTwo();
}