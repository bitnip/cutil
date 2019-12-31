#ifndef __FILE_H
#define __FILE_H
#ifdef __cplusplus
extern "C"{
#endif

#include "buffer.h"
#include "uri.h"

int fileLoad(struct URI *uri, struct Buffer *buffer);
int fileSave(struct URI *uri, struct Buffer *buffer);

#ifdef __cplusplus
}
#endif
#endif
