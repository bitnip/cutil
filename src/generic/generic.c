#include "../map/map.h"
#include "../vector/vector.h"
#include "../list/list.h"
#include "generic.h"
#include "../string.h"
#include <stddef.h>

struct Collection List = {
    {
        sizeof(struct List),
        "List",
        (void (*)(void *))listCompose,
        (void (*)(void *))listRelease,
        NULL
    },
    NULL,
    NULL,
    (struct Iterator (*)(void *))listIterator,
    listNext
};

struct Collection Map = {
    {
        sizeof(struct Map),
        "Map",
        (void (*)(void *))mapCompose,
        (void (*)(void *))mapRelease,
        NULL
    },
    (unsigned int (*)(void*,char *,void*))mapAdd,
    (void * (*)(void*,char *))mapGet,
    (struct Iterator (*)(void *))mapIterator,
    mapNext
};

struct Collection Array = {
    {
        sizeof(struct Vector),
        "Array",
        (void (*)(void *))vectorCompose,
        (void (*)(void *))vectorRelease,
        NULL
    },
    (unsigned int (*)(void*, char*, void*)) vectorAddStr,
    (void * (*)(void*, char *))vectorGetStr,
    (struct Iterator (*)(void *))vectorIterator,
    vectorNext
};

void freePtr(void **ptr) {
    free(*ptr);
}

struct Object String = {
    sizeof(char*),
    "String",
    NULL,
    (void (*)(void *))freePtr,
    (long long int (*)(void *))strHash
};

struct Object Pointer = {
    sizeof(void*),
    "Pointer",
    NULL,
    NULL,
    NULL
};

struct Object Integer = {
    sizeof(long),
    "Integer",
    NULL,
    NULL,
    NULL
};

struct Object Float = {
    sizeof(double),
    "Float",
    NULL,
    NULL,
    NULL
};

struct Object Bool = {
    sizeof(char),
    "Boolean",
    NULL,
    NULL,
    NULL
};

void* genericData(struct Generic *generic) {
    char *ptr = (char*)generic;
    ptr += sizeof(struct Generic);
    return (void*)ptr;
}

void genericRelease(struct Generic *generic) {
    if(!generic) return;
    if(generic->object->release) generic->object->release(genericData(generic));
    free(generic);
}

struct Generic *genericCompose(struct Object* object) {
    unsigned int size = sizeof(struct Generic) + (object ? object->size : 0);
    struct Generic *generic = malloc(size);

    if(!generic) return NULL;

    generic->object = object;
    if(object->setup) object->setup(genericData(generic));

    // TODO: Make this defined by type...
    struct Collection *collection = (struct Collection *)object;
    if(collection == &List) {
        struct List* list = genericData(generic);
        list->freeData = (void (*)(void *))genericRelease;
    } else if(collection == &Map) {
        struct Map* map = genericData(generic);
        map->freeData = (void (*)(void *))genericRelease;
        map->hashKey = (long long unsigned int (*)(const void *))strHash;
    } else if(collection == &Array) {
        struct Vector* vector = genericData(generic);
        vector->freeData = (void (*)(void *))genericRelease;
    }

    return generic;
}

struct Generic* genericGet(struct Generic *root, char* key) {
    if(key == NULL || root == NULL) return NULL;

    struct Collection *collection = (struct Collection *)root->object;
    if(collection != &Map && collection != &List && collection != &Array) {
        return NULL;
    }

    return collection->get(genericData(root), key);
}

unsigned int genericAdd(struct Generic *root, char* key, struct Generic *value) {
    if(root == NULL) return 0;

    struct Collection *collection = (struct Collection *)root->object;
    if(collection != &Map && collection != &List && collection != &Array) {
        return 0;
    }
    collection->add(genericData(root), key, value);
    return 1; // TODO:
}


struct Generic* getAt(struct Generic *root, char* path) {
    if(path == NULL || root == NULL) return NULL;

    char *thisToken = path, *nextDelim = NULL, *nextToken = NULL;
    while(tokenize(&thisToken, &nextDelim, &nextToken, ".")) {

        struct Collection *collection = (struct Collection *)root->object;
        if(collection != &Map && collection != &List && collection != &Array) {
            return NULL;
        }

        if(nextToken) *nextDelim = 0;
        root = collection->get(genericData(root), thisToken);
        if(nextToken) *nextDelim = '.';
    }
    return root;
}

unsigned int addAt(struct Generic *root, char* path, struct Generic *value) {
    if(path == NULL || root == NULL) return 0;

    char *thisToken = path, *nextDelim = NULL, *nextToken = NULL;
    while(tokenize(&thisToken, &nextDelim, &nextToken, ".")) {

        struct Collection *collection = (struct Collection *)root->object;
        if(collection != &Map && collection != &List && collection != &Array) {
            return 0;
        }

        if(nextToken) {
            *nextDelim = 0;
            root = collection->get(genericData(root), thisToken);
            *nextDelim = '.';
        } else {
            collection->add(genericData(root), thisToken, value);
        }
    }
    return 1;
}