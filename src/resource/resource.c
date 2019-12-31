#include "../string.h"
#include "resource.h"
#include "file.h"
#include "../error.h"

struct Scheme fileScheme = {
    fileLoad,
    fileSave
};

const char *extFromPath(const char* path) {
    const char *delim = strFindLast(path, '.');
    return delim ? delim+1 : NULL;
}

int load(
        struct ResourceAdapter *ra,
        const char *uri,
        struct Generic **output) {
    struct URI u;

    struct Generic *item = (struct Generic *)mapGet(&ra->externalResources, uri);
    if(item) {
        *output = item;
        return STATUS_OK;
    }

    if(parseURI(&u, uri)) return STATUS_INPUT_ERR;

    // Check if the protocol is recognized.
    struct Scheme* scheme = (struct Scheme*)mapGet(&ra->schemes, u.scheme);
    if(!scheme) return STATUS_SCHEME_ERR;

    // Check if the format is recognized.
    const char* ext = extFromPath(u.path);
    if(!ext) return STATUS_FORMAT_ERR; // TODO
    struct Adapter* adapter = (struct Adapter*)mapGet(&ra->adapterByExt, ext);
    if(!adapter) return STATUS_FORMAT_ERR;

    struct Buffer buffer;
    int result = scheme->load(&u, &buffer);
    if(result) return result;

    result = adapter->parse(ra, &u, &buffer, output);
    free(buffer.bytes);

    if(result) return result;

    char *key = strCopy(uri);
    mapAdd(&ra->externalResources, key, *output);

    return result;
}

int save(
        struct ResourceAdapter *ra,
        const char *uri,
        struct Generic *input) {
    struct URI u;
    if(parseURI(&u, uri)) return STATUS_INPUT_ERR;
    // Check if the protocol is recognized.
    struct Scheme* scheme = (struct Scheme*)mapGet(&ra->schemes, u.scheme);
    if(!scheme) return STATUS_SCHEME_ERR;
    // Check if the format is recognized.
    const char* ext = extFromPath(u.path);
    struct Adapter* adapter = (struct Adapter*)mapGet(&ra->adapterByExt, ext);
    if(!adapter) return STATUS_FORMAT_ERR;

    struct Buffer buffer;
    int result = adapter->unparse(ra, &u, &buffer, input);
    //printf("UNPARSE %d\n", result);
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
    mapCompose(&ra->externalResources);
    ra->externalResources.hashKey = strHash;
    mapCompose(&ra->internalResources);
    ra->internalResources.hashKey = strHash;
    ra->internalResources.freeKey = free;
    ra->internalResources.freeData = (void (*)(void *))genericRelease;
    return 0; // TODO:
}

void resourceAdapterRelease(struct ResourceAdapter *ra) {
    mapRelease(&ra->internalResources);
}
