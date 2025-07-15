#ifndef __LIST_NODE_H
#define __LIST_NODE_H

struct ListNode {
    void *data;
    struct ListNode *next;
    struct ListNode *prev;
};

struct ListNode *listNodeAlloc(const void *data);
void listNodeFree(struct ListNode *node, void (*freeData)(void*));

#endif
