#ifndef __ERROR_H
#define __ERROR_H
#ifdef __cplusplus
extern "C"{
#endif

enum STATUS_CODE {
    STATUS_OK,
    STATUS_INPUT_ERR,
    STATUS_ALLOC_ERR,
    STATUS_PARSE_ERR,
    STATUS_FOUND_ERR,
    STATUS_SCHEME_ERR,
    STATUS_FORMAT_ERR,
    STATUS_RANGE_ERR
};

#ifdef __cplusplus
}
#endif
#endif