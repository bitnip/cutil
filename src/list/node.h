#ifndef __NODE_H
#define __NODE_H

#include <stdio.h>
#include <stdlib.h>

struct Node {
    void* data;
    struct Node* next;
    struct Node* prev;
};

struct Node* nodeCreate(const void *data);
void nodeFree(struct Node* node, void (*freeData)(void*));

#endif
