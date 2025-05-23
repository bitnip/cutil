#include <stdlib.h>
#include "../error.h"
#include "../string.h"
#include "../list/list.h"
#include "uri.h"

struct URI *uriAlloc() {
    return malloc(sizeof(struct URI));
}

void uriCompose(struct URI *uri) {
    memset(uri, 0,  sizeof(struct URI));
}

static int collapsePath(char **result, const char *path) {
    // Collapse any occurances of parent directory.
    struct List folders;
    listCompose(&folders);
    folders.freeData = free;

    unsigned int pathLength = 0;
    const char *thisToken = path;
    const char *nextDelim = NULL;
    const char *nextToken = NULL;
    while(tokenize(&thisToken, &nextDelim, &nextToken, "/")) {
        unsigned int length = nextDelim-thisToken;
        if(strCmpN(thisToken, "..", 2) == 0) {
            const char *prev = listRemoveTail(&folders);
            if(prev) {
                pathLength -= strlen(prev);
            }
        } else {
            pathLength += length;
            char *folder = strCopyN(thisToken, length);
            int addResult = listAddTail(&folders, folder);
            if(addResult) {
                listRelease(&folders);
                return STATUS_ALLOC_ERR;
            }
        }
    }

    unsigned int delimCount = folders.size;
    *result = malloc(sizeof(char) * pathLength + delimCount + 1);
    if(*result) {
        struct Iterator it = listIterator(&folders);
        char *collapsedPos = *result;
        char *folder;
        unsigned int count = 0;
        while((folder = listNext(&it))) {
            unsigned int folderLen = strlen(folder);
            strCpyTo(collapsedPos, folder);
            collapsedPos+=folderLen;

            if(++count < folders.size) {
                *collapsedPos = '/';
                collapsedPos++;
            }
        }
        *collapsedPos = 0;
    }
    listRelease(&folders);
    return *result ? STATUS_OK : STATUS_ALLOC_ERR;
}

int parseURI(struct URI *output, const char *input) {
    uriCompose(output);

    // TODO: Only allow valid characters...
    const char *thisToken = (char*)input;
    const char *nextDelim = NULL;
    const char *nextToken = NULL;

    if(!tokenize(&thisToken, &nextDelim, &nextToken, ":")) return STATUS_PARSE_ERR;
    if(!nextToken) return STATUS_PARSE_ERR;

    output->scheme = strCopyN(thisToken, nextDelim - thisToken);
    if(output->scheme == NULL) {
        return STATUS_ALLOC_ERR;
    }

    thisToken = nextToken;
    const char *path = NULL;
    if(*thisToken == '/' && *(thisToken+1) == '/') {
        if(
            !tokenize(&thisToken, &nextDelim, &nextToken, "/") ||
            !tokenize(&thisToken, &nextDelim, &nextToken, "/") ||
            !tokenize(&thisToken, &nextDelim, &nextToken, "/?#")
        ) {
            uriRelease(output);
            return STATUS_PARSE_ERR;
        } 
        // TODO: Parse out.
        const char *authorityStart = thisToken;
        const char *authoritySep = (char *)strFindFirst(authorityStart, '@');
        const char *authorityEnd = nextDelim;
        // Parse out username and password.
        if(authoritySep && authorityEnd > authoritySep) {
            char *userInfoSep = (char *)strFindFirst(authorityStart, ':');
            if(userInfoSep && userInfoSep < authoritySep) {
                output->username = strCopyN(authorityStart, userInfoSep - authorityStart); // TODO: Handle failure.
                output->password = strCopyN(userInfoSep+1, authoritySep - userInfoSep - 1); // TODO: Handle failure.
            } else {
                output->username = strCopyN(authorityStart, authoritySep - authorityStart); // TODO: Handle failure.
            }
        }
        // Parse out host and port.
        // TODO: IPV6 addresses might not work.
        const char *hostStart = authoritySep ? authoritySep+1 : authorityStart;
        const char *hostSep = (char*)strFindFirst(hostStart, ':');
        if(hostSep && hostSep < authorityEnd) {
            output->host = strCopyN(hostStart, hostSep - hostStart); // TODO: Handle failure.
            output->port = strCopyN(hostSep+1, authorityEnd - hostSep - 1); // TODO: Handle failure.
        } else {
            output->host = strCopyN(hostStart, authorityEnd - hostStart); // TODO: Handle failure.
        }
        path = nextDelim;
    } else {
        path = nextToken;
    }

    if(tokenize(&thisToken, &nextDelim, &nextToken, "?#")) {
        // Extract path from URI.
        output->path = strCopyN(path, nextDelim - path);
        if(output->path == NULL) {
            uriRelease(output);
            return STATUS_ALLOC_ERR;
        }

        // Collapse parent directories.
        char *collapsedPath = NULL;
        unsigned int result = collapsePath(&collapsedPath, output->path);
        if(result) {
            uriRelease(output);
            return result;
        }
        free((char*)output->path);
        output->path = collapsedPath;
    }

    if(*nextDelim == '#') {
        output->fragment = strCopy(nextToken); // TODO: Handle failure.
        return STATUS_OK;
    }

    if(tokenize(&thisToken, &nextDelim, &nextToken, "#")) {
        output->query = strCopyN(thisToken, nextDelim - thisToken); // TODO: Handle failure.
    }
    output->fragment = strCopy(nextToken); // TODO: Handle failure.

    return STATUS_OK;
}

void uriRelease(struct URI *uri) {
    free((void*)uri->scheme);
    free((void*)uri->username);
    free((void*)uri->password);
    free((void*)uri->host);
    free((void*)uri->port);
    free((void*)uri->path);
    free((void*)uri->query);
    free((void*)uri->fragment);
}

void uriFree(struct URI *uri) {
    uriRelease(uri);
    free(uri);
}

char *uriToStr(struct URI *uri) {
    unsigned int byteCount = 0;

    char hasUserInfo = uri->username || uri->password;
    char hasAuth = hasUserInfo || uri->host || uri->port;

    if(uri->scheme) byteCount += strlen(uri->scheme) + 1;
    if(hasAuth) {
        byteCount += 2; // Authority seperator.
        if(uri->username) byteCount += strlen(uri->username);
        if(uri->password) {
            byteCount += 1; // Password seperator.
            byteCount += strlen(uri->password);
        }
        if(hasUserInfo) {
            byteCount += 1; // Userinfo seperator.
        }
        if(uri->host) byteCount += strlen(uri->host);
        if(uri->port) {
            byteCount += 1; // Port seperator.
            byteCount += strlen(uri->port);
        }
    }
    if(uri->path) byteCount += strlen(uri->path);
    if(uri->query) {
        byteCount += 1; // Query seperator.
        byteCount += strlen(uri->query);
    }
    if(uri->fragment) {
        byteCount += 1; // Fragment seperator.
        byteCount += strlen(uri->fragment);
    }

    char *ptr = malloc(byteCount + 1);
    if(ptr == NULL) return NULL;
    char *result = ptr;

    result[byteCount] = 0; // Terminate string.

    if(uri->scheme) {
        result += strCpyTo(result, uri->scheme);
        result += strCpyTo(result, ":");
    }
    if(hasAuth) {
        result += strCpyTo(result, "//"); // Authority seperator.
        if(uri->username) result += strCpyTo(result, uri->username);
        if(uri->password) {
            result += strCpyTo(result, ":"); // Password seperator.
            result += strCpyTo(result, uri->password);
        }
        if(hasUserInfo) {
            result += strCpyTo(result, "@"); // Userinfo seperator.
        }
        if(uri->host) result += strCpyTo(result, uri->host);
        if(uri->port) {
            result += strCpyTo(result, ":"); // Port seperator.
            result += strCpyTo(result, uri->port);
        }
    }
    if(uri->path) result += strCpyTo(result, uri->path);
    if(uri->query) {
        result += strCpyTo(result, "?");
        result += strCpyTo(result, uri->query);
    }
    if(uri->fragment) {
        result += strCpyTo(result, "#");
        result += strCpyTo(result, uri->fragment);
    }
    return ptr;
}

char *uriSwapExt(struct URI *input, const char *ext, unsigned char flags) {
    struct URI tmp;
    tmp.scheme = input->scheme;
    tmp.username = input->username;
    tmp.password = input->password;
    tmp.host = input->host;
    tmp.port = input->port;
    tmp.query = input->query;
    tmp.fragment = input->fragment;

    const char *dot = strFindLast(input->path, '.');
    unsigned int sansExtLength = dot ? dot - input->path : strlen(input->path);
    int newPathLength = sansExtLength + 1 + strlen(ext);
    char *tmpPath = malloc(newPathLength + 1); // TODO:
    tmp.path = tmpPath;
    tmpPath[newPathLength] = 0;
    tmpPath += strCpyNTo(tmpPath, sansExtLength, input->path);
    tmpPath += strCpyTo(tmpPath, ".");
    tmpPath += strCpyTo(tmpPath, ext);

    char *result = uriStrip(&tmp, flags);
    free((char*)tmp.path);
    return result;
}

char *uriSwapFile(struct URI *input, const char *file, unsigned char flags) {
    struct URI tmp;
    tmp.scheme = input->scheme;
    tmp.username = input->username;
    tmp.password = input->password;
    tmp.host = input->host;
    tmp.port = input->port;
    tmp.query = input->query;
    tmp.fragment = input->fragment;

    const char *slash = strFindLast(input->path, '/');
    unsigned int sansFileLength = slash ? slash - input->path : strlen(input->path);
    int newPathLength = sansFileLength + 1 + strlen(file);
    char *tmpPath = malloc(newPathLength + 1);

    tmp.path = tmpPath;
    tmpPath[newPathLength] = 0;
    tmpPath += strCpyNTo(tmpPath, sansFileLength, input->path);
    tmpPath += strCpyTo(tmpPath, "/");
    tmpPath += strCpyTo(tmpPath, file);

    char *result = uriStrip(&tmp, flags);
    free((char*)tmp.path);
    return result;
}

char *uriGetFileName(struct URI *input) {
    const char *start = strFindLast(input->path, '/');
    if(start == NULL) return NULL;
    start+=1; // Move after slash character.
    const char *end = strFindFirst(start, '.');
    return strCopyN(start, end-start);
}

char *charGetFileName(const char *input) {
    struct URI uri;
    int result;
    if((result = parseURI(&uri, input))) {
        uriRelease(&uri);
        return NULL;
    }
    const char *start = strFindLast(uri.path, '/');
    if(start == NULL) return NULL;
    start+=1; // Move after slash character.
    const char *end = strFindFirst(start, '.');
    return strCopyN(start, end-start);
}

char *uriStrip(struct URI *uri, unsigned char flags) {
    struct URI tmp;
    tmp.scheme = flags & URI_SCHEME ? NULL : uri->scheme;
    tmp.username = flags & URI_USERNAME ? NULL : uri->username;
    tmp.password = flags & URI_PASSWORD ? NULL : uri->password;
    tmp.host = flags & URI_HOST ? NULL : uri->host;
    tmp.port = flags & URI_PORT ? NULL : uri->port;
    tmp.path = flags & URI_PATH ? NULL : uri->path;
    tmp.query = flags & URI_QUERY ? NULL : uri->query;
    tmp.fragment = flags & URI_FRAGMENT ? NULL : uri->fragment;
    return uriToStr(&tmp);
}