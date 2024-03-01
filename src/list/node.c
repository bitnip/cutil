#include "node.h"
#include <stdlib.h>

struct ListNode *nodeCreate(const void *data) {
    struct ListNode *node = (struct ListNode*)malloc(sizeof(struct ListNode));
    node->next = NULL;
    node->prev = NULL;
    node->data = (void*)data;
    return node;
}

void nodeFree (struct ListNode *node, void (*freeData)(void*)) {
    if(node == NULL) return;
    if(freeData != NULL) {
        (*freeData)(node->data);
    }
    free(node);
}
