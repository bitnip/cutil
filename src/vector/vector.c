#include "vector.h"
#include "../integer/integer.h"
#include "../error.h"

void vectorRelease(struct Vector *vector) {
    if(vector == NULL) return;
    if(vector->freeData) {
        for(int i = 0; i < vector->size; i++) {
            vector->freeData(vector->items[i]);
        }
    }
    free(vector->items);
}

void vectorFree(struct Vector *vector) {
    vectorRelease(vector);
    free(vector);
}

int vectorCompose(struct Vector *vector) {
    vector->size = 0;
    vector->allocSize = 0;
    vector->freeData = NULL;
    vector->items = NULL;
    return STATUS_OK;
}

struct Vector *vectorAlloc() {
    struct Vector *vector = malloc(sizeof(struct Vector));
    if(vector == NULL) return NULL;
    vectorCompose(vector);
    return vector;
}


int vectorAddTail(struct Vector *vector, void *item) {
    if(vector->allocSize <= vector->size) {
        vector->allocSize = vector->allocSize ? vector->allocSize * 2 : 2;
        void *tmp = realloc(vector->items, vector->allocSize * sizeof(void *));
        if(tmp == NULL) return STATUS_ALLOC_ERR;
        vector->items = tmp;
    }
    vector->items[vector->size++] = item;
    return STATUS_OK;
}

void *vectorRemoveTail(struct Vector *vector) {
    if(!vector->size) return NULL;
    void* value = vector->items[--vector->size];
    //TODO: Resize down?
    //vector->items = realloc(vector->items, vector->size * sizeof(void*));
    return value;
}

unsigned int vectorSize(const struct Vector *vector) {
    return vector->size;
}

void *vectorGet(const struct Vector *vector, int key) {
    if(key < 0) {
        key = vector->size + key + 1;
    }
    if(key >= 0 && key < vector->size) {
        return vector->items[key];
    }
    return NULL;
}

void *vectorGetTail(const struct Vector *vector) {
    if(!vector->size) return NULL;
    return vector->items[vector->size-1];
}

int vectorAddStr(struct Vector *vector, const char *key, void *item) {
    long index;
    int result = longParse(key, 10, &index);
    if(result) return result;
    return vectorAdd(vector, index, item);
}

void *vectorGetStr(struct Vector *vector, const char *input) {
    long index;
    int result = longParse(input, 10, &index);
    if(result) return NULL;
    return vectorGet(vector, index);
}

int vectorIsEmpty(const struct Vector *vector) {
    return vector->size == 0;
}

int vectorAdd(struct Vector *vector, int key, void *value) {
    /* Adds value at key, frees value at current position. */
    if(key < 0) {
        key = vector->size + key + 1;
    }
    if(key >= 0 && key < vector->size) {
        void *temp = vectorGet(vector, key);
        if(temp && vector->freeData) {
            vector->freeData(temp);
        }
        *(vector->items + key) = value;
        return STATUS_OK;
    } else if(key == vector->size) {
        return vectorAddTail(vector, value);
    }
    return STATUS_INPUT_ERR;
}

int vectorInsert(struct Vector *vector, int key, void *value) {
    if(key < 0) {
        key = vector->size + key + 1;
    }
    if(key >= 0 && key < vector->size) {
        // Add the final element to the end of the array.
        int result = vectorAddTail(vector, vectorGetTail(vector));
        if(result) return result;
        // Shift elements after key forward.
        for(int i = vector->size - 2; i > key; i--) {
            vector->items[i] = vector->items[i-1];
        }
        vector->items[key] = value;
        return STATUS_OK;
    } else if(key == vector->size) {
        return vectorAddTail(vector, value);
    }
    return 1;
}

void *vectorRemove(struct Vector *vector, int key) {
    /*Remove k, shift remaining elements back.*/
    if(key < 0) {
        key = vector->size + key + 1;
    }
    if(key >= 0 && key < vector->size - 1) {
        void *temp = vectorGet(vector, key);
        for(int i = key; i < vector->size - 1; i++) {
            vector->items[i] = vector->items[i+1];
        }
        vector->size--;
        return temp;
    } else if(key == vector->size - 1) {
        return vectorRemoveTail(vector);
    }
    return NULL;
}

struct Iterator vectorIterator(const struct Vector *vector) {
    struct Iterator iterator;
    iterator.collection = (struct Vector*)vector;
    iterator.index = 0;
    return iterator;
}

void* vectorNext(struct Iterator* iterator) {
    struct Vector* vector = iterator->collection;
    if(iterator->index >= vector->size) return NULL;
    void *element = vector->items[iterator->index++];
    return element;
}