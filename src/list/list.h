#ifndef __LIST_H
#define __LIST_H
#ifdef __cplusplus
extern "C"{
#endif

#include "../iterator.h"
#include "node.h"

struct List {
    int size;
    struct Node* head;
    struct Node* tail;
    int (*compare)(const void *, const void *);
    char *(*toString)(const void *);
    void (*freeData)(void *);
};

int listCompose(struct List *);
void listRelease(struct List *);
void listFree(struct List *);
long long listHash(struct List *);
int listSize(const struct List *);
int listIsEmpty(const struct List *);
int listAddTail(struct List *, const void *);
struct Iterator listIterator(struct List *);
struct Iterator listIteratorMode(struct List *, enum ITERATION_MODE);
void *listNext(struct Iterator *);
void *listCurrent(struct Iterator *);
void *listPopCurrent(struct Iterator *);
void *listSwapCurrent(struct Iterator *itr, void *data);
int listAddCurrent(struct Iterator *, void *);
void listPrint(const struct List* list);

int listContains(const struct List *, const void *);
void listAddHead(struct List *, const void *);
void *listHead(struct List *);
void *listTail(struct List *);
void *listRemoveHead(struct List *);
void *listRemoveTail(struct List *);
void listReverse(struct List *);
void listAppend(struct List *, struct List *);

#ifdef __cplusplus
}
#endif
#endif
