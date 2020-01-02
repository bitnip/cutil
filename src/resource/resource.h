#ifndef __RESOURCE_H
#define __RESOURCE_H
#ifdef __cplusplus
extern "C"{
#endif

#include "../generic/generic.h"
#include "../map/map.h"
#include "buffer.h"
#include "uri.h"

struct ResourceAdapter {
    struct Map schemes;
    struct Map adapterByExt;
    struct Map externalResources;
};
int resourceAdapterCompose(struct ResourceAdapter *);
void resourceAdapterRelease(struct ResourceAdapter *);

enum ResourceStatus {
    RES_STATUS_ERROR, // data set to error code.
    RES_STATUS_CACHE, // data points to cache uri if different than uri.
    RES_STATUS_ADAPT, // data is being converted.
    RES_STATUS_PARSE, // data is being parsed.
    RES_STATUS_FETCH, // data is being loaded.
    RES_STATUS_START  // data is staged for loading.
};

struct Resource {
    const char *uri;
    void *data;
    enum ResourceStatus status;
};

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

#ifdef __cplusplus
}
#endif
#endif
