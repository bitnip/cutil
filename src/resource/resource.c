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
    struct Generic *item = (struct Generic*)mapGet(&ra->resources, fileURI);
    if(item) {
        uriRelease(&u);
        free(fileURI);
        return getSubResource(u, item, output);
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
    struct Adapter *adapter = (struct Adapter*)mapGet(&ra->adapterByExt, ext);
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
    result = mapAdd(&ra->resources, fileURI, item);
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
    struct Adapter* adapter = (struct Adapter*)mapGet(&ra->adapterByExt, ext);
    if(!adapter) return STATUS_FORMAT_ERR;

    struct Buffer buffer;
    int result = adapter->unparse(ra, &u, &buffer, input);
    if(result) return result;

    result = scheme->save(&u, &buffer);
    free(buffer.bytes);
    return result;
}

int resourceAdapterCompose(struct ResourceAdapter *ra) {
    mapCompose(&ra->schemes);
    ra->schemes.hashKey = strHash;
    mapCompose(&ra->adapterByExt);
    ra->adapterByExt.hashKey = strHash;
    mapCompose(&ra->resources);
    ra->resources.hashKey = strHash;
    ra->resources.freeData = (void (*)(void*))genericRelease;
    ra->resources.freeKey = free;
    return STATUS_OK;
}

void resourceAdapterRelease(struct ResourceAdapter *ra) {
    mapRelease(&ra->resources);
    mapRelease(&ra->adapterByExt);
    mapRelease(&ra->schemes);
}
