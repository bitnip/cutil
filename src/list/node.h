#ifndef __LIST_NODE_H
#define __LIST_NODE_H

struct ListNode {
    void *data;
    struct ListNode *next;
    struct ListNode *prev;
};

struct ListNode *nodeCreate(const void *data);
void nodeFree(struct ListNode *node, void (*freeData)(void*));

#endif
