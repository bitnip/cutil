#include <stdio.h>
#include <stdlib.h>
#include "file.h"
#include "../error.h"

#define READ_CHUCK_BYTES 1048576 // 1MiB
int fileLoad(struct URI *uri, struct Buffer *buffer) {
    FILE *fp = fopen(uri->path+1, "rb");
    if(fp == NULL) return STATUS_FOUND_ERR;

    char  *data = NULL, *temp;
    size_t bufferSize = 0;
    size_t used = 0;
    while(1) {
        bufferSize = used + READ_CHUCK_BYTES + 1;
        if( // Check for overflow.
            bufferSize <= used ||
            // Resize buffer to fit next read chunk.
            !(temp = realloc(data, bufferSize))
        ) {
            free(data);
            return STATUS_ALLOC_ERR;
        }

        data = temp;

        size_t n = fread(data + used, 1, READ_CHUCK_BYTES, fp);
        used+=n;
        if(n < READ_CHUCK_BYTES) break;
    }

    if (ferror(fp)) {
        free(data);
        return STATUS_FOUND_ERR;
    }

    // Could realloc(data, used+1)
    // but buffer is freed after parsing anyway.

    data[used] = '\0';

    buffer->byteCount = used;
    buffer->bytes = data;

    return STATUS_OK;
}

int fileSave(struct URI *uri, struct Buffer *buffer){
    /*Write `buffer` to `uri`.*/
    FILE *fp;
    if((fp = fopen(uri->path+1, "wb+"))) {
        fwrite(buffer->bytes, 1, buffer->byteCount, fp);
        fclose(fp);
        return STATUS_OK;
    }
    return STATUS_FOUND_ERR;
}