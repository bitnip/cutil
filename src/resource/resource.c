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

int getSubResource(
        struct URI uri,
        struct Generic *input,
        struct Generic **output) {
    if(uri.fragment == NULL) {
        *output = input;
        return STATUS_OK;
    }
    if(input->object == &Map.object) {
        *output = genericGet(input, (char*)uri.fragment); // TODO Fix cast
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

    // Create a URI without the fragment and query portion.
    char *fileURI = uriStrip(&u, URI_QUERY | URI_FRAGMENT);

    struct Generic *item = (struct Generic *)mapGet(
        &ra->externalResources, fileURI);
    if(item) {
        free(fileURI);
        return getSubResource(u, item, output);
    }

    // Check if the protocol is recognized.
    struct Scheme* scheme = (struct Scheme*)mapGet(&ra->schemes, u.scheme);
    if(!scheme) return STATUS_SCHEME_ERR;

    // Check if the format is recognized.
    const char* ext = extFromPath(u.path);
    if(!ext) return STATUS_FORMAT_ERR; // TODO
    struct Adapter *adapter = (struct Adapter*)mapGet(&ra->adapterByExt, ext);
    if(!adapter) return STATUS_FORMAT_ERR;

    struct Buffer buffer;
    int result = scheme->load(&u, &buffer);
    if(result) return result;

    result = adapter->parse(ra, &u, &buffer, &item);
    free(buffer.bytes);

    if(result) return result;

    mapAdd(&ra->externalResources, fileURI, item);
    free(fileURI);

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
    ra->externalResources.freeData = (void (*)(void *))genericRelease;
    return 0; // TODO:
}

void resourceAdapterRelease(struct ResourceAdapter *ra) {
    mapRelease(&ra->externalResources);
}
