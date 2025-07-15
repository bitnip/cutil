#include "node.h"
#include <stdlib.h>

struct ListNode *listNodeAlloc(const void *data) {
    struct ListNode *node = (struct ListNode*)malloc(sizeof(struct ListNode));
    node->data = (void*)data;
    return node;
}

void listNodeFree (struct ListNode *node, void (*freeData)(void*)) {
    if(node == NULL) return;
    if(freeData != NULL) {
        (*freeData)(node->data);
    }
    free(node);
}
