#include "map.h"
#include "../error.h"

unsigned long long ptrHash(const void *ptr) {
    unsigned long long key = (unsigned long long)ptr;
    return key;
}

unsigned long primes[] = {2ul, 5ul, 11ul, 23ul, 47ul, 97ul, 197ul, 397ul};

void mapRelease(struct Map* map) {
    if(!map) return;
    if(map->freeData || map->freeKey) {
        // MapPair doesn't have a pointer to freeData.
        // Each List Node's MapPair needs to be free'd first.
        struct Iterator it =  mapIterator(map);
        void *value = NULL;
        while((value = mapNext(&it))) {
            const void *key = mapKey(&it);
            if(map->freeData) map->freeData(value);
            if(map->freeKey) map->freeKey((void*)key);
        }
    }
    // The rest will be cleaned up safely by the destructors.
    vectorRelease(&map->buckets);
}

int mapCompose(struct Map *map) {
    int result;
    if((result = vectorCompose(&map->buckets))) {
        return result;
    }
    map->size = 0;
    map->buckets.freeData = (void (*)(void *))vectorFree;
    map->freeData = NULL;
    map->hashKey = NULL;
    map->freeKey = NULL;
    return STATUS_OK;
}

int mapIsEmpty(const struct Map* map) {
    return map->size == 0;
}

void* mapAddPair(struct Map* map, struct MapPair* pair) {
    unsigned long bucketIndex = pair->hash % vectorSize(&map->buckets);
    struct Vector* bucket = vectorGet(&map->buckets, bucketIndex);
    struct Iterator iterator = vectorIterator(bucket);

    struct MapPair* currentPair = NULL;
    while((currentPair = vectorNext(&iterator))) {
        if(currentPair->hash == pair->hash) {
            void* oldValue = pair->value;
            currentPair->value = pair->value;
            return oldValue;
        }
    }
    map->size++;
    vectorAddTail(bucket, pair);
    return NULL;
}

void mapSizeUp(struct Map* map) {
    /*Resize bucket count to satisfy >= N+1 and <= a prime near 2N+1.
    TODO: Resize map down when removing elements.*/

    if(map->size < vectorSize(&map->buckets)) return;
    unsigned long* endPtr = primes + sizeof(primes) / sizeof(unsigned long) - 1;
    unsigned long* nextSizePtr = primes;
    while(*nextSizePtr <= vectorSize(&map->buckets) && nextSizePtr < endPtr) {
        nextSizePtr++;
    }

    struct List tempList;
    listCompose(&tempList);
    struct MapPair* pair = NULL;

    for(int bucketIndex = 0; bucketIndex < vectorSize(&map->buckets); bucketIndex++) {
        struct Vector* bucket = vectorGet(&map->buckets, bucketIndex);
        while((pair = vectorRemoveTail(bucket))) {
            listAddTail(&tempList, pair);
        }
    }
    unsigned long nextSize = *nextSizePtr;
    while(vectorSize(&map->buckets) < nextSize) {
        /* TODO: This would be more memory effiencint if:
        - Each collection didn't have its own function pointers.
        -- Reduces the cost of each element by as much as >8 bytes * 2N.
        - Used a contiguous array of buckets instead of bucket pointers.
        -- Reduces the cost of each element by as much as 8 bytes * 2N.
        -- Would increase the largest allocated block of memory by a factor of 2N*sizeof(n).
        */
        struct Vector *newBucket = malloc(sizeof(struct Vector));
        vectorCompose(newBucket);
        newBucket->freeData = free;
        vectorAddTail(&map->buckets, newBucket);
    }
    map->size = 0;
    while((pair = listRemoveHead(&tempList))) {
        mapAddPair(map, pair);
    }
    listRelease(&tempList);
}

void* mapAdd(struct Map *map, const void *key, void *value) {
    // TODO: Fix return value.
    if(!(map && map->hashKey)) return NULL;
    mapSizeUp(map);
    struct MapPair *pair = malloc(sizeof(struct MapPair));
    pair->hash = map->hashKey(key);
    pair->key = key;
    pair->value = value;
    return mapAddPair(map, pair);
}

unsigned int mapSize(struct Map *map) {
    return map->size;
}

void *mapGet(struct Map *map, const void *key) {
    if(!vectorSize(&map->buckets)) return NULL;
    unsigned long long hash = map->hashKey(key);
    unsigned long bucketIndex = hash % vectorSize(&map->buckets);
    struct Vector *bucket = vectorGet(&map->buckets, bucketIndex);
    struct Iterator iterator = vectorIterator(bucket);
    struct MapPair *pair;
    while((pair = vectorNext(&iterator))) {
        if(pair->hash == hash) {
            return pair->value;
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

    struct Vector *bucket;
    while((bucket = vectorGet(&map->buckets, bucketIndex))) {
        struct MapPair *pair = vectorGet(bucket, elementIndex);
        if(pair == NULL) {
            bucketIndex++;
            elementIndex = 0;
            continue;
        }
        return pair->key;
    }
    return NULL;
}

void* mapNext(struct Iterator* iterator) {
    struct Map* map = iterator->collection;

    struct Vector* bucket;
    while((bucket = vectorGet(&map->buckets, iterator->index))) {
        unsigned long long elementIndex = (unsigned long long)iterator->current;
        struct MapPair *pair = vectorGet(bucket, elementIndex);
        if(pair == NULL) {
            iterator->index++;
            iterator->current = 0;
            continue;
        }
        iterator->current = (void*)(++elementIndex);
        return pair->value;
    }
    return NULL;
}