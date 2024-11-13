#include <stdlib.h>
#include "../string.h"
#include "resource.h"
#include "file.h"
#include "../error.h"

struct Scheme fileScheme = {
    fileLoad,
    fileSave
};

const char *extFromPath(const char *path) {
    const char *delim = strFindLast(path, '.');
    return delim ? delim + 1 : NULL;
}

int getSubResource(
        struct URI uri,
        struct Generic *input,
        struct Generic **output) {
    if(uri.fragment == NULL) {
        *output = input;
        return STATUS_OK;
    }
    if(input->object == &Map.object) {
        *output = genericGet(input, uri.fragment);
        return *output == NULL ? STATUS_FOUND_ERR : STATUS_OK;
    }
    return STATUS_INPUT_ERR;
}



int load(
        struct ResourceAdapter *ra,
        const char *uri,
        struct Generic **output) {
    struct URI u;
    if(parseURI(&u, uri)) return STATUS_INPUT_ERR;

    // Create a URI for the main resource.
    char *fileURI = uriStrip(&u, URI_QUERY | URI_FRAGMENT);

    // Check if the resource is already loaded.
    struct Generic *item = (struct Generic*)mapGet(&ra->uriToGeneric, fileURI);
    if(item) {
        free(fileURI);
        int result = getSubResource(u, item, output);
        uriRelease(&u);
        return result;
    }

    // Check if the protocol is recognized.
    struct Scheme *scheme = (struct Scheme*)mapGet(&ra->schemes, u.scheme);
    if(!scheme) {
        uriRelease(&u);
        free(fileURI);
        return STATUS_SCHEME_ERR;
    }

    // Check if a file extension exists.
    const char *ext = extFromPath(u.path);
    if(!ext) {
        uriRelease(&u);
        free(fileURI);
        return STATUS_FORMAT_ERR;
    }

    // Check if an adapter exists for this extension.
    struct Adapter *adapter = (struct Adapter*)mapGet(&ra->extToAdapter, ext);
    if(!adapter) {
        uriRelease(&u);
        free(fileURI);
        return STATUS_FORMAT_ERR;
    }

    // Load resource into buffer.
    struct Buffer buffer;
    int result = scheme->load(&u, &buffer);
    if(result) {
        uriRelease(&u);
        free(fileURI);
        return result;
    }
    // Parse resource.
    result = adapter->parse(ra, &u, &buffer, &item);
    free(buffer.bytes);
    if(result) {
        uriRelease(&u);
        free(fileURI);
        return result;
    }

    // Track loaded resource.
    result = mapAdd(&ra->uriToGeneric, fileURI, item);
    if(result) {
        uriRelease(&u);
        free(fileURI);
        return result;
    }

    // Get the requested file fragment.
    result = getSubResource(u, item, output);

    uriRelease(&u);
    return result;
}

int save(
        struct ResourceAdapter *ra,
        const char *uri,
        struct Generic *input) {
    struct URI u;
    if(parseURI(&u, uri)) return STATUS_INPUT_ERR;
    // Check if the protocol is recognized.
    struct Scheme *scheme = (struct Scheme*)mapGet(&ra->schemes, u.scheme);
    if(!scheme) return STATUS_SCHEME_ERR;
    // Check if the format is recognized.
    const char *ext = extFromPath(u.path);
    struct Adapter* adapter = (struct Adapter*)mapGet(&ra->extToAdapter, ext);
    if(!adapter) return STATUS_FORMAT_ERR;

    struct Buffer buffer;
    int result = adapter->unparse(ra, &u, &buffer, input);
    if(result) return result;

    result = scheme->save(&u, &buffer);
    free(buffer.bytes);
    return result;
}

struct Resource *resourceAlloc() {
    struct Resource *r = malloc(sizeof(struct Resource));
    r->cache = NULL;
    r->status = RES_STATUS_LOADING;
    uriCompose(&r->uri);
    return r;
}

void resourceFree(struct Resource *r) {
    uriRelease(&r->uri);
    free(r);
}

int resourceAdapterCompose(struct ResourceAdapter *ra) {
    mapCompose(&ra->schemes);
    ra->schemes.hashKey = strHash;
    mapCompose(&ra->extToAdapter);
    ra->extToAdapter.hashKey = strHash;
    mapCompose(&ra->uriToGeneric);
    ra->uriToGeneric.hashKey = strHash;
    ra->uriToGeneric.freeData = (void (*)(void*))genericRelease;
    ra->uriToGeneric.freeKey = free;
    mapCompose(&ra->uriToResource);
    ra->uriToResource.hashKey = strHash;
    ra->uriToResource.freeData = (void (*)(void*))resourceFree;
    ra->uriToResource.freeKey = free;
    ra->statusCallback = NULL;
    ra->statusContext = NULL;
    return STATUS_OK;
}

void resourceAdapterRelease(struct ResourceAdapter *ra) {
    mapRelease(&ra->uriToResource);
    mapRelease(&ra->uriToGeneric);
    mapRelease(&ra->extToAdapter);
    mapRelease(&ra->schemes);
    ra->statusCallback = NULL;
    ra->statusContext = NULL;
}

struct Resource *resourceLoad(struct ResourceAdapter *ra, const char *uri) {
    struct URI u;
    if(parseURI(&u, uri)) return NULL; //return STATUS_INPUT_ERR;

    // Check if the resource is already loaded.
    struct Resource *resource = (struct Resource*)mapGet(&ra->uriToResource, uri);
    if(resource) {
        uriRelease(&u);
    } else {
        resource = resourceAlloc();
        if(resource) {
            resource->uri = u;
            resource->status = RES_STATUS_LOADING;
            if(mapAdd(&ra->uriToResource, strCopy(uri), resource)) { // TODO:
                resourceFree(resource);
                uriRelease(&u);
            }
        } else {
            uriRelease(&u);
        }
    }
    return resource;
}

struct Resource *resourceSave(struct ResourceAdapter *ra, const char *uri) {
    struct URI u;
    if(parseURI(&u, uri)) return NULL; //return STATUS_INPUT_ERR;

    // Check if the resource is already loaded.
    struct Resource *resource = (struct Resource*)mapGet(&ra->uriToResource, uri);
    if(resource) {
        uriRelease(&u);
    } else {
        resource = resourceAlloc();
        if(resource) {
            resource->uri = u;
            resource->status = RES_STATUS_SAVING;
            if(mapAdd(&ra->uriToResource, strCopy(uri), resource)) { // TODO:
                resourceFree(resource);
                return NULL;
            }
        } else {
            uriRelease(&u);
        }
    }
    return resource;
}

void resourceUpdate(struct ResourceAdapter *ra) {
    struct Iterator it = mapIterator(&ra->uriToResource);
    struct Resource *resource;
    while((resource = mapNext(&it))) {
        int result;
        if(resource->status == RES_STATUS_LOADING) {
            if(ra->statusCallback) ra->statusCallback(ra->statusContext, resource, resource->status);

            char *uri = uriToStr(&resource->uri); // TODO:
            result = load(ra, uri, &resource->cache);
            free(uri);
            resource->status = result ? RES_STATUS_UNLOADED : RES_STATUS_LOADED;

            if(ra->statusCallback) ra->statusCallback(ra->statusContext, resource, resource->status);
            break;
        } else if(resource->status == RES_STATUS_SAVING) {
            if(ra->statusCallback) ra->statusCallback(ra->statusContext, resource, resource->status);

            char *uri = uriToStr(&resource->uri); // TODO:
            result = save(ra, uri, resource->cache);
            free(uri);
            resource->status = result ? RES_STATUS_UNSAVED : RES_STATUS_LOADED;

            if(ra->statusCallback) ra->statusCallback(ra->statusContext, resource, resource->status);
            break;
        }
    }
}