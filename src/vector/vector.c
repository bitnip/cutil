#include "vector.h"
#include "../error.h"

void vectorRelease(struct Vector* vector) {
    if(vector == NULL) return;
    if(vector->freeData) {
        for(int i = 0; i < vector->size; i++) {
            vector->freeData(vector->items[i]);
        }
    }
    free(vector->items);
}

void vectorFree(struct Vector* vector) {
    vectorRelease(vector);
    free(vector);
}

int vectorCompose(struct Vector* vector) {
    vector->size = 0;
    vector->allocSize = 0;
    vector->freeData = NULL;
    vector->items = NULL;
    return STATUS_OK;
}

int vectorAddTail(struct Vector *vector, void *item) {
    if(vector->allocSize <= vector->size) {
        vector->allocSize = vector->allocSize ? vector->allocSize*2 : 2;
        vector->items = realloc(vector->items, vector->allocSize * sizeof(void*));
        if(vector->items == NULL) return 1;
    }
    vector->items[vector->size++] = item;
    return 0;
}

void* vectorRemoveTail(struct Vector* vector) {
    if(!vector->size) return NULL;
    void* value = vector->items[--vector->size];
    //TODO: Resize down?
    //vector->items = realloc(vector->items, vector->size * sizeof(void*));
    return value;
}

unsigned int vectorSize(const struct Vector* vector) {
    return vector->size;
}

void *vectorGet(const struct Vector* vector, unsigned int index) {
    if(index>=vector->size) return NULL;
    return vector->items[index];
}

void *vectorGetTail(const struct Vector* vector) {
    if(!vector->size) return NULL;
    return vector->items[vector->size-1];
}

unsigned int vectorAddStr(struct Vector* vector, char* input, void* item) {
    // TODO: Use input as index?
    vectorAddTail(vector, item);
    return 1;
}

void *vectorGetStr(struct Vector *vector, char *input) {
    unsigned int index = atoi(input);
    if(index>=vector->size) return NULL;
    return vector->items[index];
}

int vectorIsEmpty(const struct Vector *vector) {
    return vector->size == 0;
}

int vectorAdd(struct Vector *vector, unsigned int key, void *value) {
    if(key < vector->size) {
        void *temp = vectorGet(vector, key);
        if(temp && vector->freeData) {
            vector->freeData(temp);
        }
        *(vector->items + key) = value;
        return 0;
    } else if(key == vector->size) {
        return vectorAddTail(vector, value);
    }
    return 1;
}

int vectorInsert(struct Vector *vector, unsigned int key, void *value) {
    if(key < vector->size) {
        // Add the final element to the end of the array.
        int result = vectorAddTail(vector, vectorGetTail(vector));
        if(result) return result;
        // Shift elements after key forward.
        for(int i = vector->size - 2; i > key; i--) {
            vector->items[i] = vector->items[i-1];
        }
        vector->items[key] = value;
        return 0;
    } else if(key == vector->size) {
        return vectorAddTail(vector, value);
    }
    return 1;
}

void *vectorRemove(struct Vector *vector, unsigned int key) {
    /*Remove k, shift remaining elements back.*/
    if(key < vector->size - 1) {
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

struct Iterator vectorIterator(const struct Vector* vector) {
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