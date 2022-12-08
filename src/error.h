#ifndef __ERROR_H
#define __ERROR_H
#ifdef __cplusplus
extern "C"{
#endif

enum STATUS_CODE {
    STATUS_OK,         // Completed without issues.
    STATUS_INPUT_ERR,  // Invalid parameters.
    STATUS_ALLOC_ERR,  // Memory could not be reserved.
    STATUS_PARSE_ERR,  // Input could not be understood.
    STATUS_FOUND_ERR,  // Resource not found.
    STATUS_SCHEME_ERR, // TODO: Write accurate description or merge with another.
    STATUS_FORMAT_ERR, // TODO: Write accurate description or merge with another.
    STATUS_RANGE_ERR   // Value was out of allowed range.
};

#ifdef __cplusplus
}
#endif
#endif