#include <stdlib.h>
#include "../map/map.h"
#include "../vector/vector.h"
#include "../list/list.h"
#include "generic.h"
#include "../string.h"
#include "../error.h"

struct Collection List = {
    {
        sizeof(struct List),
        "List",
        (void (*)(void*))listCompose,
        (void (*)(void*))listRelease,
        NULL
    },
    NULL,
    NULL,
    (struct Iterator (*)(void*))listIterator,
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
    (unsigned int (*)(void*, const char*, void*))mapAdd,
    (void *(*)(void*, const char*))mapGet,
    (struct Iterator (*)(void*))mapIterator,
    mapNext
};

struct Collection Array = {
    {
        sizeof(struct Vector),
        "Array",
        (void (*)(void*))vectorCompose,
        (void (*)(void*))vectorRelease,
        NULL
    },
    (unsigned int (*)(void*, const char*, void*)) vectorAddStr,
    (void *(*)(void*, const char*))vectorGetStr,
    (struct Iterator (*)(void*))vectorIterator,
    vectorNext
};

void freePtr(void **ptr) {
    free(*ptr);
}

struct Object String = {
    sizeof(char*),
    "String",
    NULL,
    (void (*)(void*))freePtr,
    (long long int (*)(void*))strHash
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
    sizeof(float),
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

char PATH_SEPARATOR[2] = ".";

void *genericData(struct Generic *generic) {
    char *ptr = (char*)generic;
    ptr += sizeof(struct Generic);
    return (void*)ptr;
}
#include <stdio.h>
void genericRelease(struct Generic *generic) {
    if(!generic) return;
    if(generic->object->release) {
        void* data = genericData(generic);
        generic->object->release(data);
        printf("%s %p\n", generic->object->name, (void *)genericData(generic));
    }
    free(generic);
}

struct Generic *genericCompose(struct Object *object) {
    unsigned int size = sizeof(struct Generic) + (object ? object->size : 0);
    struct Generic *generic = malloc(size);

    if(!generic) return NULL;

    generic->object = object;
    if(object->setup) object->setup(genericData(generic));

    // TODO: Handle as part of generics.
    struct Collection *collection = (struct Collection*)object;
    if(collection == &List) {
        struct List *list = genericData(generic);
        list->freeData = (void (*)(void*))genericRelease;
    } else if(collection == &Map) {
        struct Map *map = genericData(generic);
        map->freeData = (void (*)(void*))genericRelease;
        map->hashKey = (long long unsigned int (*)(const void*))strHash;
    } else if(collection == &Array) {
        struct Vector* vector = genericData(generic);
        vector->freeData = (void (*)(void*))genericRelease;
    }

    return generic;
}

struct Generic *genericGet(struct Generic *root, const char *key) {
    if(key == NULL || root == NULL) return NULL;

    struct Collection *collection = (struct Collection*)root->object;
    if(collection != &Map && collection != &List && collection != &Array) {
        return NULL;
    }

    return collection->get(genericData(root), key);
}

unsigned int genericAdd(struct Generic *root, const char *key, struct Generic *value) {
    if(root == NULL) return STATUS_FORMAT_ERR;

    // TODO: Handle as part of generics.
    struct Collection* collection = (struct Collection*)root->object;
    if(collection != &Map && collection != &List && collection != &Array) {
        return STATUS_FORMAT_ERR;
    }

    char *newKey = strCopy(key);
    if(!newKey) return STATUS_ALLOC_ERR;

    return collection->add(genericData(root), strCopy(key), value);
}

struct Generic* genericGetNative(struct Generic *root, struct Object *type, void *value, const char *key) {
    struct Generic* valueGeneric = genericGet(root, key);
    if(valueGeneric) {
        if(valueGeneric->object != type) {
            return NULL;
        }
        memcpy(value, genericData(valueGeneric), type->size);
    }
    return valueGeneric;
}

unsigned int genericAddNative(struct Generic *root, struct Object *type, const void *value, const char *key) {
    struct Generic *valueGeneric = genericCompose(type); // TODO: Handle alloc failure.
    memcpy(genericData(valueGeneric), value, type->size);
    return genericAdd(root, key, valueGeneric);
}

struct Generic *getAt(struct Generic *root, const char *path) {
    if(path == NULL || root == NULL) return NULL;

    const char *thisToken = path, *nextDelim = NULL, *nextToken = NULL;
    while(tokenize(&thisToken, &nextDelim, &nextToken, PATH_SEPARATOR)) {

        // TODO: Handle as part of generics.
        struct Collection *collection = (struct Collection*)root->object;
        if(collection != &Map && collection != &List && collection != &Array) {
            return NULL;
        }

        if(nextToken) {
            char *key = strCopyN(thisToken, nextDelim - thisToken);
            root = collection->get(genericData(root), key);
            free(key);
        } else {
            root = collection->get(genericData(root), thisToken);
        }
    }
    return root;
}

unsigned int addAt(struct Generic *root, const char *path, struct Generic *value) {
    if(path == NULL) return STATUS_INPUT_ERR;

    const char *thisToken = path, *nextDelim = NULL, *nextToken = NULL;
    while(tokenize(&thisToken, &nextDelim, &nextToken, PATH_SEPARATOR)) {
        if(root == NULL) return STATUS_FOUND_ERR;
        // Handle as part of generics.
        struct Collection *collection = (struct Collection*)root->object;
        if(collection != &Map && collection != &List && collection != &Array) {
            return STATUS_FOUND_ERR;
        }

        char *key = strCopyN(thisToken, nextDelim - thisToken);
        if(nextToken) {
            root = collection->get(genericData(root), key);
        } else {
            int result = collection->add(genericData(root), key, value);
            if(result) return result;
        }
    }
    return STATUS_OK;
}