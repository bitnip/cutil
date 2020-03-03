#ifndef __GENERIC_H
#define __GENERIC_H
#ifdef __cplusplus
extern "C"{
#endif

struct Object {
    unsigned int size;
    const char *name;
    void (*setup)(void*);
    void (*release)(void*);
    long long (*hash)(void*);
};

struct Collection {
    struct Object object;
    unsigned int (*add)(void*, const char*, void*);
    void* (*get)(void*, const char*);
    struct Iterator (*iterator)(void*);
    void* (*next)(struct Iterator*);
};

extern struct Collection Array;
extern struct Collection List;
extern struct Collection Map;
extern struct Object String;
extern struct Object Integer;
extern struct Object Float;
extern struct Object Bool;
extern struct Object Pointer;

void *objectCompose();
void objectRelease(void* object);
long long objectHash(void* object);

struct Generic {
    struct Object* object;
};


void *genericData(struct Generic *generic);
struct Generic *genericGet(struct Generic *root, const char *key);
unsigned int genericAdd(struct Generic *root, const char *key, struct Generic *value);

struct Generic *getAt(struct Generic *root, char *path);
unsigned int addAt(struct Generic *root, char *path, struct Generic *value);

void genericRelease(struct Generic *element);
struct Generic *genericCompose(struct Object* object);

#ifdef __cplusplus
}
#endif
#endif