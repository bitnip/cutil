#include "node.h"

struct Node *nodeCreate(const void *data) {
    struct Node *node = (struct Node *) malloc(sizeof(struct Node));
    node->next = NULL;
    node->prev = NULL;
    node->data = (void *)data;
    return node;
}

void nodeFree (struct Node *node, void (*freeData)(void*)) {
    if(node == NULL) return;
    if(freeData != NULL)
    (*freeData)(node->data);
    free(node);
}
