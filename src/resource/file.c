#include <stdio.h>
#include <stdlib.h>
#include "file.h"
#include "../error.h"

int fileLoad(struct URI *uri, struct Buffer *buffer) {
    FILE *fp;
    int result = STATUS_OK;
    if((fp = fopen(uri->path+1, "rb"))) {
        fseek(fp, 0L, SEEK_END);
        buffer->byteCount = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        if((buffer->bytes = (char*)malloc(buffer->byteCount+1))) {
            unsigned int readCount = fread(
                buffer->bytes, sizeof(char), buffer->byteCount, fp);
            if(readCount==buffer->byteCount) {
                result = STATUS_OK;
                buffer->bytes[buffer->byteCount] = 0;
            } else {
                result = STATUS_INPUT_ERR;
                free(buffer->bytes);
            }
        } else {
            result = STATUS_ALLOC_ERR;
        }
        fclose(fp);
    } else {
        result = STATUS_FOUND_ERR;
    }
    return result;
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