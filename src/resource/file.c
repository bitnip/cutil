#include <stdio.h>
#include <stdlib.h>
#include "file.h"
#include "../error.h"

int fileLoad(struct URI *uri, struct Buffer *buffer) {
    FILE* fp;
    if((fp = fopen(uri->path+1, "rb"))) {
        fseek(fp, 0L, SEEK_END);
        buffer->byteCount = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        if((buffer->bytes = (char*)malloc(buffer->byteCount+1))) {
            unsigned int readCount = fread(
                buffer->bytes, sizeof(char), buffer->byteCount, fp);
            buffer->bytes[buffer->byteCount] = 0;
            if(readCount==buffer->byteCount) {
                return STATUS_OK;
            } else {
                free(buffer->bytes);
            }
        } else {
            return STATUS_ALLOC_ERR;
        }
    }
    return STATUS_FOUND_ERR;
}

int fileSave(struct URI *uri, struct Buffer *buffer){
    /*Write `buffer` to `uri`.*/
    FILE *fp;
    if((fp = fopen(uri->path+1, "wb+"))) {
        fwrite(buffer->bytes, 1, buffer->byteCount, fp);
        //fputs(buffer, fp);
        fclose(fp);
        return STATUS_OK;
    }
    return STATUS_FOUND_ERR;
}