#ifndef __URI_H
#define __URI_H
#ifdef __cplusplus
extern "C"{
#endif

#define URI_SCHEME 1 << 0
#define URI_USERNAME 1 << 1
#define URI_PASSWORD 1 << 2
#define URI_HOST 1 << 3
#define URI_PORT 1 << 4
#define URI_PATH 1 << 5
#define URI_QUERY 1 << 6
#define URI_FRAGMENT 1 << 7
#define URI_USERINFO URI_USERNAME | URI_PASSWORD
#define URI_AUTHORITY URI_USERINFO | URI_HOST | URI_PORT

struct URI {
    const char *scheme;
    const char *username;
    const char *password;
    const char *host;
    const char *port;
    const char *path;
    const char *query;
    const char *fragment;
};

int parseURI(struct URI *output, const char *input);
char *uriToStr(struct URI *uri);
char *uriSwapExt(struct URI *input, const char *ext, unsigned char flags);
char *uriSwapFile(struct URI *input, const char *file, unsigned char flags);
void uriRelease(struct URI *uri);
char *charGetFileName(const char *input);
char *uriStrip(struct URI *uri, unsigned char flags);

#ifdef __cplusplus
}
#endif
#endif