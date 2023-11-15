#ifndef __NODE_H
#define __NODE_H

struct Node {
    void *data;
    struct Node *next;
    struct Node *prev;
};

struct Node *nodeCreate(const void *data);
void nodeFree(struct Node *node, void (*freeData)(void*));

#endif
