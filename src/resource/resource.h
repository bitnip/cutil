#ifndef __RESOURCE_H
#define __RESOURCE_H
#ifdef __cplusplus
extern "C"{
#endif

#include "../generic/generic.h"
#include "../map/map.h"
#include "buffer.h"
#include "uri.h"

enum ResourceStatus {
    RES_STATUS_LOADING, // Resource is being loaded.
    RES_STATUS_SAVING,  // Resource is being saved.
    RES_STATUS_UNLOADED,// Resource is not loaded.
    RES_STATUS_LOADED,  // Resource is loaded and saved.
    RES_STATUS_UNSAVED  // Resource is loaded but unsaved.
};
struct Resource {
    struct URI uri;
    struct Generic *cache;
    enum ResourceStatus status;
};

struct ResourceAdapter {
    struct Map schemes;       // { char *ext : struct Scheme * }
    struct Map extToAdapter;  // { char *ext : struct Adapter * }
    struct Map uriToGeneric;  // { char *uri : { char *name : struct Generic * } }
    struct Map uriToResource; // { char *uri : struct Resource * }
    void (*statusCallback)(
        void *statusContext,
        struct Resource *resource,
        enum ResourceStatus status);
    void *statusContext;
};
int resourceAdapterCompose(struct ResourceAdapter *);
void resourceAdapterRelease(struct ResourceAdapter *);

struct Scheme {
    int (*load)(struct URI *uri, struct Buffer *buffer);
    int (*save)(struct URI *uri, struct Buffer *buffer);
};
extern struct Scheme fileScheme;

struct Adapter {
    int (*parse)(
        struct ResourceAdapter *ra,
        struct URI *uri,
        struct Buffer *buffer,
        struct Generic **data);
    int (*unparse)(
        struct ResourceAdapter *ra,
        struct URI *uri,
        struct Buffer *buffer,
        struct Generic *data);
};

int load(struct ResourceAdapter *ra, const char *uri, struct Generic **output);
int save(struct ResourceAdapter *ra, const char *uri, struct Generic *input);

struct Resource *resourceAlloc();
void resourceFree(struct Resource *r);
struct Resource *resourceLoad(struct ResourceAdapter *ra, const char *uri);
struct Resource *resourceSave(struct ResourceAdapter *ra, const char *uri);
void resourceUpdate(struct ResourceAdapter *ra);

#ifdef __cplusplus
}
#endif
#endif
