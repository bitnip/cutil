#ifndef __MAP_H
#define __MAP_H
#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include "../list/list.h"
#include "../vector/vector.h"
#include "../iterator.h"

struct Map {
    unsigned int size;
    struct Vector buckets;
    void (*freeData)(void *);
    unsigned long long (*hashKey)(const void *);
    void (*freeKey)(void *);
};

struct MapPair {
    unsigned long long hash;
    const void* key;
    void* value;
};

unsigned long long ptrHash(const void * ptr); // TODO: Move

int mapCompose(struct Map* map);
void mapRelease(struct Map* map);
unsigned int mapSize(struct Map* map);
int mapIsEmpty(const struct Map* map);
void* mapAdd(struct Map* map, const void* key, void* item);
void* mapGet(struct Map* map, const void* value);

const void* mapKey(struct Iterator* iterator);
struct Iterator mapIterator(struct Map*);
void* mapNext(struct Iterator*);

#ifdef __cplusplus
}
#endif
#endif
