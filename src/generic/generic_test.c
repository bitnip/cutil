#include "../map/map.h"
#include "../vector/vector.h"
#include "../list/list.h"
#include "generic.h"
#include "../assertion.h"
#include "../error.h"

void addMapToMapTest() {
    struct Generic *base = genericCompose(&Map.object);
    char paths[][5] = {
        "a",
        "a.aa",
        "b",
        "b.bb",
        "c"
    };
    unsigned int result;
    for (unsigned int i = 0; i < 5; i++) {
        result = addAt(base, paths[i], genericCompose(&Map.object));
        assertIntegersEqual(result, STATUS_OK);
    }
    assertIntegersEqual(((struct Map*)genericData(base))->size, 3);
    genericRelease(base);
}

void addVectorToVectorTest() {
    struct Generic *base = genericCompose(&Array.object);
    char paths[][5] = {
        "0",
        "0.0",
        "1",
        "1.0",
        "2"
    };
    unsigned int result;
    for (unsigned int i = 0; i < 5; i++) {
        result = addAt(base, paths[i], genericCompose(&Array.object));
        assertIntegersEqual(result, STATUS_OK);
    }
    // TODO: add collection size function.
    assertIntegersEqual(((struct Map*)genericData(base))->size, 3);
    genericRelease(base);
}

void genericGetAtTest() {
    struct Generic *base = genericCompose(&Map.object);
    char paths[][5] = {
        "a",
        "a.0",
        "b",
        "b.0",
        "c"
    };
    unsigned int result;
    for (unsigned int i = 0; i < 5; i++) {
        result = addAt(base, paths[i], genericCompose(i&1 ? &Array.object : &Map.object));
        assertIntegersEqual(result, STATUS_OK);
    }

    struct Generic *value = getAt(base, "");
    assertIsNull(value);

    for (unsigned int i = 0; i < 5; i++) {
        value = getAt(base, paths[i]);
        assertNotNull(value);
        assertPointersEqual(value->object, i&1 ? &Array.object : &Map.object);
    }

    genericRelease(base);
}

void genericTest() {
    addMapToMapTest();
    addVectorToVectorTest();
    genericGetAtTest();
}
