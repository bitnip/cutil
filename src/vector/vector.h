#ifndef __VECTOR_H
#define __VECTOR_H
#ifdef __cplusplus
extern "C"{
#endif

#include "../iterator.h"

struct Vector {
    unsigned int size;
    unsigned int allocSize;
    void **items;
    void (*freeData)(void*);
};

void vectorFree(struct Vector *vector);
int vectorCompose(struct Vector *vector);
struct Vector* vectorAlloc();
void vectorRelease(struct Vector *vector);
int vectorAddTail(struct Vector *vector, void *item);
int vectorAdd(struct Vector *vector, int key, void *value);
int vectorInsert(struct Vector *vector, int key, void *value);
void *vectorRemoveTail(struct Vector *vector);
void *vectorRemove(struct Vector *vector, int key);
unsigned int vectorSize(const struct Vector *vector);
int vectorIsEmpty(const struct Vector *vector);
void *vectorGetTail(const struct Vector *vector);
void *vectorGet(const struct Vector *vector, int key);
void *vectorGetStr(struct Vector *vector, const char *input);
int vectorAddStr(struct Vector *vector, const char *input, void *item);
void vectorRelease(struct Vector *vector);

struct Iterator vectorIterator(const struct Vector *vector);
void *vectorNext(struct Iterator *iterator);
void *vectorPopCurrent(struct Iterator*);

#ifdef __cplusplus
}
#endif
#endif
