#ifndef __BUFFER_H
#define __BUFFER_H
#ifdef __cplusplus
extern "C"{
#endif

struct Buffer {
    unsigned int byteCount;
    char *bytes;
};

#ifdef __cplusplus
}
#endif
#endif
