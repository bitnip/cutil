#ifndef __VECTOR_H
#define __VECTOR_H
#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include "../iterator.h"

struct Vector {
    unsigned int size;
    unsigned int allocSize;
    void **items;
    void (*freeData)(void *);
};

void vectorFree(struct Vector *vector);
int vectorCompose(struct Vector *vector);
void vectorRelease(struct Vector *vector);
int vectorAddTail(struct Vector *vector, void *item);
int vectorAdd(struct Vector *vector, unsigned int key, void *value);
int vectorInsert(struct Vector *vector, unsigned int key, void *value);
void* vectorRemoveTail(struct Vector*);
void* vectorRemove(struct Vector *vector, unsigned int key);
unsigned int vectorSize(const struct Vector* vector);
int vectorIsEmpty(const struct Vector* vector);
void *vectorGetTail(const struct Vector* vector);
void* vectorGet(const struct Vector* vector, unsigned int index);
void* vectorGetStr(struct Vector* vector, char* input);
unsigned int vectorAddStr(struct Vector* vector, char* input, void* item);
void vectorRelease(struct Vector* vector);

struct Iterator vectorIterator(const struct Vector*);
void* vectorNext(struct Iterator*);

#ifdef __cplusplus
}
#endif
#endif
