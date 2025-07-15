#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "../error.h"

#include "node.h"

void listRelease(struct List *list) {
    if(list == NULL) return;
    while(list->head != NULL) {
        struct ListNode *temp = list->head;
        list->head = list->head->next;
        listNodeFree(temp, list->freeData);
        list->size--;
    }
}

void listFree(struct List *list) {
    if(list == NULL) return;
    listRelease(list);
    free(list);
}

int listCompose(struct List *list) {
    memset(list, 0, sizeof(struct List));
    return STATUS_OK;
}

struct List *listAlloc() {
    struct List *list = malloc(sizeof(struct List));
    if(list == NULL) return NULL;
    listCompose(list);
    return list;
}

int listSize(const struct List *list) {
    return list->size;
}

int listIsEmpty(const struct List *list) {
    return list->size == 0;
}

void listAddHead(struct List *list, const void *data) {
    if(list == NULL || data == NULL) return;

    struct ListNode *node = listNodeAlloc(data);

    list->size++;
    if(list->head == NULL) {
        list->tail = node;
    } else {
        list->head->prev = node;
    }
    node->next = list->head;
    node->prev = NULL;
    list->head = node;
}

static void listAddTailNode(struct List *list, struct ListNode *node) {
    list->size++;
    if(list->tail) {
        list->tail->next = node;
    } else {
        list->head = node;
    }
    node->prev = list->tail;
    node->next = NULL;
    list->tail = node;
}

int listAddTail(struct List *list, const void *data) {
    if(list == NULL || data == NULL) return STATUS_INPUT_ERR;

    struct ListNode *node = listNodeAlloc(data);
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

    if(list->head == NULL) {
        list->tail = NULL;
    } else {
        list->head->prev = NULL;
    }

    listNodeFree(node, NULL);
    return data;
}

void *listRemoveTail(struct List *list) {
    if(list == NULL || list->tail == NULL) return NULL;

    list->size--;
    struct ListNode *node = list->tail;
    void *data = node->data;
    node->data = NULL;

    list->tail = list->tail->prev;

    if(list->tail == NULL) {
        list->head = NULL;
    } else {
        list->tail->next = NULL;
    }

    listNodeFree(node, NULL);
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
    if(itr->current) return itr->current;

    // Default based on iteration mode.
    struct List *list = itr->collection;
    switch(itr->mode) {
        case ITERATION_FORWARD:
            itr->current = list->head;
            break;
        case ITERATION_REVERSE:
            itr->current = list->tail;
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

    int advance = itr->current != NULL;
    struct ListNode *current = listCurrentNode(itr);
    if(advance) {
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
        itr->current = current;
    }

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
    struct ListNode *node = listNodeAlloc(data);
    if(!node) return STATUS_ALLOC_ERR;
    if(current) {
        switch(itr->mode) {
            case ITERATION_REVERSE:
                /*Place node before current*/
                if(list->head == current) {
                    list->head = node;
                    node->prev = NULL;
                } else {
                    node->prev = current->prev;
                    current->prev->next = node;
                }
                current->prev = node;
                node->next = current;
                break;
            default:
            //case ITERATION_FORWARD:
                /*Place node after current*/
                if(list->tail == current) {
                    list->tail = node;
                    node->next = NULL;
                } else {
                    node->next = current->next;
                    current->next->prev = node;
                }
                current->next = node;
                node->prev = current;
                break;
        }
        list->size++;
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
    listNodeFree(node, NULL);
    list->size--;
    return data;
}