#include "../string.h"
#include "uri.h"

int parseURI(struct URI *output, const char *input) {
    output->scheme = NULL;
    output->authority.userinfo = NULL;
    output->authority.host = NULL;
    output->authority.port = NULL;
    output->path = NULL;
    output->query = NULL;
    output->fragment = NULL;
    // TODO: Only allow valid characters...
    char *thisToken = (char*)input;
    char *nextDelim = NULL;
    char *nextToken = NULL;

    if(!tokenize(&thisToken, &nextDelim, &nextToken, ":")) return 1;
    if(!nextToken) return 1;
    output->scheme = strCopyN(thisToken, nextDelim - thisToken);
    //printf("Scheme: %s\n", output->scheme);

    thisToken = nextToken;
    //if(!tokenize(&thisToken, &nextDelim, &nextToken, ":")) return 1;

    char *path = NULL;
    //printf("AAA: %s\n", thisToken);

    if(*thisToken == '/' && *(thisToken+1) == '/') {
        //printf("EEE: %s %c %s\n", thisToken, *nextDelim, nextToken);

        if(!tokenize(&thisToken, &nextDelim, &nextToken, "/")) return 1;
        //printf("BBB: %s\n", thisToken);
        if(!tokenize(&thisToken, &nextDelim, &nextToken, "/")) return 1;
        //printf("CCC: %s\n", thisToken);
        if(!tokenize(&thisToken, &nextDelim, &nextToken, "/")) return 1;
        //printf("DDD: %s\n", thisToken);
        // TODO: Parse out.
        output->authority.host = strCopyN(thisToken, nextDelim - thisToken);
        //printf("Host: %s\n", output->authority.host);
        path = nextDelim;
    } else {
        path = nextToken;
    }

    if(!tokenize(&thisToken, &nextDelim, &nextToken, "?")) return 1;

    output->path = strCopyN(path, (nextDelim+1) - thisToken);
    //printf("Path: %s\n", output->path);

    if(tokenize(&thisToken, &nextDelim, &nextToken, "#")) {
        output->query = strCopyN(thisToken, nextDelim - thisToken);
        output->fragment = strCopy(nextToken);
    }

    return 0;
}

void freeURI(struct URI *uri) {
    if(uri->scheme) free((void*)uri->scheme);
    if(uri->authority.userinfo) free((void*)uri->authority.userinfo);
    if(uri->authority.host) free((void*)uri->authority.host);
    if(uri->authority.port) free((void*)uri->authority.port);
    if(uri->path) free((void*)uri->path);
    if(uri->query) free((void*)uri->query);
    if(uri->fragment) free((void*)uri->fragment);
}

char *uriToStr(struct URI *uri) {
    unsigned int byteCount = 0; // ... `:` ...
    if(uri->scheme) byteCount += strlen(uri->scheme) + 1;

    if(uri->authority.host) {
        byteCount += strlen(uri->authority.host) + 2; // ... // ...
        if(uri->authority.userinfo) byteCount += strlen(uri->authority.userinfo) + 1; // ... @ ...
        if(uri->authority.port) byteCount += strlen(uri->authority.port) + 1; // ... : ...
    }

    if(uri->path) byteCount += strlen(uri->path);

    if(uri->query) byteCount += strlen(uri->query) + 1; // ... ? ...
    if(uri->fragment) byteCount += strlen(uri->fragment) + 1; // ... # ...

    char *ptr = malloc(byteCount + 1);
    char *result = ptr;

    result[byteCount] = 0;

    if(uri->scheme) {
        result += strCpyTo(result, uri->scheme);
        result += strCpyTo(result, ":");
    }
    if(uri->authority.host) {
        result += strCpyTo(result, "//");
        if(uri->authority.userinfo) {
            result += strCpyTo(result, uri->authority.userinfo);
            result += strCpyTo(result, "@");
        }
        result += strCpyTo(result, uri->authority.host);
        if(uri->authority.port) {
            result += strCpyTo(result, ":");
            result += strCpyTo(result, uri->authority.port);
        }
    }
    if(uri->path) {
        result += strCpyTo(result, uri->path);
    }
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

char *uriSwapExt(struct URI *input, const char *ext) {
    struct URI tmp;
    tmp.scheme = input->scheme;
    tmp.authority.userinfo = input->authority.userinfo;
    tmp.authority.host = input->authority.host;
    tmp.authority.port = input->authority.port;

    const char *dot = strFindLast(input->path, '.');
    unsigned int sansExtLength = dot ? dot - input->path : strlen(input->path);
    int newPathLength = sansExtLength + 1 + strlen(ext);
    char *tmpPath = malloc(newPathLength + 1);
    tmp.path = tmpPath;
    tmpPath[newPathLength] = 0;
    tmpPath += strCpyNTo(tmpPath, sansExtLength, input->path);
    tmpPath += strCpyTo(tmpPath, ".");
    tmpPath += strCpyTo(tmpPath, ext);

    tmp.query = input->query;
    tmp.fragment = input->fragment;

    char *result = uriToStr(&tmp);
    free((char*)tmp.path);
    return result;
}

char *uriSwapFile(struct URI *input, const char *file) {
    struct URI tmp;
    tmp.scheme = input->scheme;
    tmp.authority.userinfo = input->authority.userinfo;
    tmp.authority.host = input->authority.host;
    tmp.authority.port = input->authority.port;

    const char *slash = strFindLast(input->path, '/');
    unsigned int sansFileLength = slash ? slash - input->path : strlen(input->path);
    int newPathLength = sansFileLength + 1 + strlen(file);
    char *tmpPath = malloc(newPathLength + 1);
    tmp.path = tmpPath;
    tmpPath[newPathLength] = 0;
    tmpPath += strCpyNTo(tmpPath, sansFileLength, input->path);
    tmpPath += strCpyTo(tmpPath, "/");
    tmpPath += strCpyTo(tmpPath, file);

    tmp.query = input->query;
    tmp.fragment = input->fragment;

    char *result = uriToStr(&tmp);
    free((char*)tmp.path);
    return result;
}