#ifndef __URI_H
#define __URI_H
#ifdef __cplusplus
extern "C"{
#endif

struct URI {
    const char *scheme;
    struct {
        const char *userinfo;
        const char *host;
        const char *port;
    } authority;
    const char *path;
    const char *query;
    const char *fragment;
};

int parseURI(struct URI *output, const char *input);
char *uriToStr(struct URI *uri);
char *uriSwapExt(struct URI *input, const char *ext);
char *uriSwapFile(struct URI *input, const char *file);
void freeURI(struct URI *uri);

#ifdef __cplusplus
}
#endif
#endif