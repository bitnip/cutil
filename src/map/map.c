#include <stdlib.h>
#include "map.h"
#include "../error.h"

struct MapPair {
    unsigned long long hash;
    const void *key;
    void *value;
};

unsigned long long ptrHash(const void *ptr) {
    unsigned long long key = (unsigned long long)ptr;
    return key;
}

unsigned long primes[] = {
    2ul,
    5ul,
    11ul,
    23ul,
    47ul,
    97ul,
    193ul,
    389ul,
    769ul,
    1543ul,
    3089ul,
    6199ul,
    12401ul,
    24799ul,
    49597ul,
    99194ul
};

void mapRelease(struct Map *map) {
    if(!map) return;
    // MapPair doesn't have a pointer to freeData.
    // Each List Node's MapPair needs to be free'd first.
    struct Vector *bucket;
    struct Iterator bucketIt = vectorIterator(&map->buckets);
    while((bucket = vectorNext(&bucketIt))) {
        struct Iterator pairIt = vectorIterator(bucket);
        struct MapPair *pair = NULL;
        while((pair = vectorNext(&pairIt))) {
            if(map->freeData) {
                map->freeData(pair->value);
            }
            if(map->freeKey) {
                map->freeKey((void*)pair->key);
            }
            free(pair);
        }
    }
    // The rest will be cleaned up safely by the destructors.
    vectorRelease(&map->buckets);
}

int mapCompose(struct Map *map) {
    int result = vectorCompose(&map->buckets);
    if(result) return result;
    map->buckets.freeData = (void (*)(void *))vectorFree;
    map->size = 0;
    map->freeData = NULL;
    map->hashKey = NULL;
    map->freeKey = NULL;
    return STATUS_OK;
}

int mapAddPair(
        struct Vector *buckets,
        unsigned int *size,
        struct MapPair *pair,
        struct MapPair **replaced) {
    unsigned long bucketIndex = pair->hash % vectorSize(buckets);
    struct Vector *bucket = vectorGet(buckets, bucketIndex);

    struct MapPair *currentPair = NULL;
    int pairIndex = 0;
    struct Iterator pairIt = vectorIterator(bucket);
    while((currentPair = vectorNext(&pairIt))) {
        if(currentPair->hash == pair->hash) {
            *replaced = currentPair;
            bucket->items[pairIndex] = pair;
            return STATUS_OK;
        }
        pairIndex++;
    }

    int result = vectorAddTail(bucket, pair);
    if(result) {
        return result;
    }

    *replaced = NULL;
    (*size)++;
    return STATUS_OK;
}

int mapSizeUp(struct Map *map) {
    if(map->size < vectorSize(&map->buckets)*4) return STATUS_OK;
    /*Resize bucket count to satisfy >= N+1 and <= a prime near 2N+1.*/
    unsigned long *endPtr = primes + sizeof(primes) / sizeof(unsigned long);
    unsigned long *nextSizePtr = primes;
    while(*nextSizePtr <= vectorSize(&map->buckets) && nextSizePtr < endPtr) {
        nextSizePtr++;
    }
    // Build new buckets.
    struct Vector newBuckets;
    vectorCompose(&newBuckets);
    newBuckets.freeData = (void (*)(void *))vectorFree;

    while(vectorSize(&newBuckets) < *nextSizePtr) {
        struct Vector* newBucket = vectorAlloc();
        if(newBucket == NULL) {
            vectorRelease(&newBuckets);
            return STATUS_ALLOC_ERR;
        }
        int result = vectorAddTail(&newBuckets, newBucket);
        if(result) {
            vectorFree(newBucket);
            vectorRelease(&newBuckets);
            return STATUS_ALLOC_ERR;
        }
    }

    // Insert pairs into new buckets.
    unsigned int newSize = 0;
    struct Vector *bucket;
    struct MapPair *pair;
    struct Iterator bucketIt = vectorIterator(&map->buckets);
    while((bucket = vectorNext(&bucketIt))) {
        struct Iterator pairIt = vectorIterator(bucket);
        while((pair = vectorNext(&pairIt))) {
            struct MapPair *replaced; /* All elements already in a map. */
            int result = mapAddPair(&newBuckets, &newSize, pair, &replaced);
            if(result) {
                vectorRelease(&newBuckets);
                return result;
            }
        }
    }
    struct Vector oldBuckets = map->buckets;
    // Swap new buckets into map.
    map->buckets = newBuckets;
    map->size = newSize;

    // Free old buckets.
    struct Iterator oldBucketIt = vectorIterator(&oldBuckets);
    while((bucket = vectorNext(&oldBucketIt))) {
        bucket->freeData = NULL;
    }
    vectorRelease(&oldBuckets);
    return STATUS_OK;
}

int mapAdd(struct Map *map, const void *key, void *value) {
    if(!(map && map->hashKey)) return STATUS_INPUT_ERR;

    int result = mapSizeUp(map);
    if(result) {
        return result;
    }

    struct MapPair *pair = malloc(sizeof(struct MapPair));
    if(!pair) {
        return STATUS_ALLOC_ERR;
    }
    pair->hash = map->hashKey(key);
    pair->key = key;
    pair->value = value;

    struct MapPair *replaced;
    result = mapAddPair(&map->buckets, &map->size, pair, &replaced);
    if(result) {
        return result;
    }
    if(replaced) {
        if(map->freeKey) map->freeKey((void*)replaced->key);
        if(map->freeData) map->freeData(replaced->value);
        free(replaced);
    }
    return STATUS_OK;
}

unsigned int mapSize(struct Map *map) {
    return map->size;
}

struct MapPair *mapGetPair(struct Map *map, const void *key) {
    if(!vectorSize(&map->buckets)) return NULL;
    unsigned long long hash = map->hashKey(key);
    unsigned long bucketIndex = hash % vectorSize(&map->buckets);
    struct Vector *bucket = vectorGet(&map->buckets, bucketIndex);
    struct Iterator iterator = vectorIterator(bucket);
    struct MapPair *pair;
    while((pair = vectorNext(&iterator))) {
        if(pair->hash == hash) {
            return pair;
        }
    }
    return NULL;
}

void *mapGet(struct Map *map, const void *key) {
    struct MapPair *pair = mapGetPair(map, key);
    return pair ? pair->value : NULL;
}

void *mapRemove(struct Map *map, const void *key) {
    if(!vectorSize(&map->buckets)) return NULL;
    unsigned long long hash = map->hashKey(key);
    unsigned long bucketIndex = hash % vectorSize(&map->buckets);
    struct Vector* bucket = vectorGet(&map->buckets, bucketIndex);
    struct Iterator iterator = vectorIterator(bucket);
    struct MapPair* pair;
    while((pair = vectorNext(&iterator))) {
        if(pair->hash == hash) {
            iterator.index -= 1;
            vectorPopCurrent(&iterator);
            map->size -= 1;
            void *value = pair->value;
            if(map->freeKey) map->freeKey((void*)pair->key);
            free(pair);
            return value;
        }
    }
    return NULL;
}

struct Iterator mapIterator(struct Map *map) {
    struct Iterator iterator;
    iterator.collection = map;
    iterator.index = 0;
    iterator.current = 0;
    return iterator;
}

const void *mapKey(struct Iterator *iterator) {
    struct Map *map = iterator->collection;
    unsigned int bucketIndex = iterator->index;
    unsigned long long elementIndex = (unsigned long long)iterator->current;

    struct Vector *bucket = vectorGet(&map->buckets, bucketIndex);
    while(bucket) {
        struct MapPair *pair = vectorGet(bucket, elementIndex);
        if(pair == NULL) {
            bucket = vectorGet(&map->buckets, ++bucketIndex);
            elementIndex = 0;
            continue;
        }
        return pair->key;
    }
    return NULL;
}

void *mapNext(struct Iterator *iterator) {
    struct Map *map = iterator->collection;
    struct Vector *bucket = vectorGet(&map->buckets, iterator->index);
    while(bucket) {
        unsigned long long elementIndex = (unsigned long long)iterator->current;
        struct MapPair *pair = vectorGet(bucket, elementIndex);
        if(pair == NULL) {
            iterator->index = iterator->index + 1;
            bucket = vectorGet(&map->buckets, iterator->index);
            iterator->current = 0;
            continue;
        }
        iterator->current = (void*)(++elementIndex);
        return pair->value;
    }
    return NULL;
}