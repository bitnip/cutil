#include <stdlib.h>
#include "list.h"
#include "../error.h"

#include "node.h"

void listRelease(struct List *list) {
    if(list == NULL) return;
    while(list->head != NULL) {
        struct ListNode *temp = list->head;
        list->head = list->head->next;
        nodeFree(temp, list->freeData);
        list->size--;
    }
}

void listFree(struct List *list) {
    if(list == NULL) return;
    listRelease(list);
    free(list);
}

int listCompose(struct List *list) {
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    list->compare = NULL;
    list->toString = NULL;
    list->freeData = NULL;
    return STATUS_OK;
}

int listSize(const struct List *list) {
    return list->size;
}

int listIsEmpty(const struct List *list) {
    return list->size == 0;
}

void listAddHead(struct List *list, const void *data) {
    if(list == NULL || data == NULL) return;

    struct ListNode* node;
    node = nodeCreate(data);

    list->size++;
    node->next = list->head;
    node->prev = NULL;

    if(list->head == NULL)
        list->tail = node;
    else
        list->head->prev = node;

    list->head = node;
}

static void listAddTailNode(struct List *list, struct ListNode *node) {
    list->size++;
    if(list->tail) {
        node->prev = list->tail;
        list->tail->next = node;
    }
    else {
        list->head = node;
    }
    list->tail = node;
}

int listAddTail(struct List *list, const void *data) {
    if(list == NULL || data == NULL) return 1;

    struct ListNode *node = nodeCreate(data);
    if(!node) return STATUS_ALLOC_ERR;

    listAddTailNode(list, node);
    return STATUS_OK;
}

void *listRemoveHead(struct List *list) {
    if(list == NULL || list->head == NULL) return NULL;

    list->size--;
    struct ListNode *node = list->head;
    void *data = node->data;
    node->data = NULL;

    list->head = list->head->next;

    if(list->head == NULL)
        list->tail = NULL;
    else
        list->head->prev = NULL;

    nodeFree(node, NULL);

    return data;
}

void *listRemoveTail(struct List *list) {
    if(list == NULL || list->tail == NULL) return NULL;

    list->size--;
    struct ListNode *node = list->tail;
    void *data = node->data;
    node->data = NULL;

    list->tail = list->tail->prev;

    if(list->tail == NULL)
        list->head = NULL;
    else
        list->tail->next = NULL;

    nodeFree(node, NULL);

    return data;
}

int listContains(const struct List *list, const void *data) {
    if(list == NULL || data == NULL) return 0;

    struct ListNode *curr = list->head;
    while(curr != NULL) {
        if(list->compare( curr->data, data ) == 0 ) {
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}

static struct ListNode *reverse(struct ListNode *oldHead) {
    struct ListNode *newHead = NULL;
    while(oldHead != NULL) {
        struct ListNode *current = oldHead;
        oldHead = oldHead->next;

        if(oldHead != NULL)
            oldHead->prev = current;

        current->next = newHead;
        current->prev = oldHead;
        newHead = current;
    }
    return newHead;
}

void listReverse(struct List *list) {
    if(list == NULL) return;
    list->tail = list->head;
    list->head = reverse(list->head);
}

void listAppend(struct List *a, struct List *b) {
    if(a == NULL || b == NULL) return;
    if(a->tail) {
        a->tail->next = b->head;
        if(b->tail) {
            a->tail = b->tail;
        }
    }
    else {
        a->head = b->head;
        a->tail = b->tail;
    }
    b->head = b->tail = NULL;
    a->size += b->size;
    b->size = 0;
}

struct Iterator listIteratorMode(struct List *list, enum ITERATION_MODE mode) {
    struct Iterator iterator;
    iterator.mode = list ? mode : ITERATION_DONE;
    iterator.collection = list;
    iterator.current = NULL;
    return iterator;
}

struct Iterator listIterator(struct List *list) {
    return listIteratorMode(list, ITERATION_FORWARD);
}

static struct ListNode *listCurrentNode(struct Iterator *itr) {
    /* Current node or set current node to new default. */
    if(!itr) return NULL;
    struct List* list = itr->collection;
    if(itr->current) return itr->current;
    // Default based on iteration mode.
    switch(itr->mode) {
        case ITERATION_FORWARD:
            if(list->head) {
                itr->current = list->head;
            } else {
                //itr->mode = ITERATION_DONE;
                break;
            }
            break;
        case ITERATION_REVERSE:
            if(list->tail) {
                itr->current = list->tail;
            } else {
                //itr->mode = ITERATION_DONE;
                break;
            }
            break;
        default:
            itr->mode = ITERATION_DONE;
    }
    return itr->current;
}

void *listCurrent(struct Iterator *itr) {
    struct ListNode *current = listCurrentNode(itr);
    return current ? current->data : NULL;
}

void *listNext(struct Iterator *itr) {
    if(!itr) return NULL;
    if(itr->mode == ITERATION_DONE) return NULL;

    struct ListNode *current = itr->current;
    if(current) {
        switch(itr->mode) {
            case ITERATION_FORWARD:
                current = current->next;
                break;
            case ITERATION_REVERSE:
                current = current->prev;
                break;
            default:
                current = NULL;
        }
    } else {
        current = listCurrentNode(itr);
    }

    itr->current = current;
    if(current) {
        return current->data;
    } else {
        itr->mode = ITERATION_DONE;
        return NULL;
    }
}

void *listSwapCurrent(struct Iterator *itr, void *data) {
    struct ListNode *node = listCurrentNode(itr);
    if(!node) return NULL;
    void *temp = node->data;
    node->data = data;
    return temp;
}

int listAddCurrent(struct Iterator *itr, void *data) {
    struct List *list = itr->collection;
    struct ListNode *current = listCurrentNode(itr);
    struct ListNode *node = nodeCreate(data);
    if(!node) return STATUS_ALLOC_ERR;
    if(current) {
        switch(itr->mode) {
            case ITERATION_FORWARD:
                /*Place node after current*/
                if(list->tail == current) {
                    list->tail = node;
                } else {
                    node->next = current->next;
                    current->next->prev = node;
                }
                current->next = node;
                node->prev = current;
                list->size++;
                break;
            case ITERATION_REVERSE:
                /*Place node before current*/
                if(list->head == current) {
                    list->head = node;
                } else {
                    node->prev = current->prev;
                    current->prev->next = node;
                }
                current->prev = node;
                node->next = current;
                list->size++;
                break;
            default:
                break;
        }
    } else {
        listAddTailNode(list, node);
    }
    return STATUS_OK;
}

void *listPopCurrent(struct Iterator *itr) {
    struct List *list = itr->collection;
    struct ListNode *node = listCurrentNode(itr);
    if(!node) return NULL;

    listNext(itr);

    if(list->head == node) {
        list->head = node->next;
    } else {
        node->prev->next = node->next;
    }

    if(list->tail == node) {
        list->tail = node->prev;
    } else {
        node->next->prev = node->prev;
    }

    void *data = node->data;
    node->data = NULL;
    nodeFree(node, NULL);
    list->size--;
    return data;
}