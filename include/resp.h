#ifndef RESP_H
#define RESP_H
#include <stddef.h>
#include <stdint.h>

typedef enum {
    respBulkString,
    respInteger
} respTypes;

typedef struct {
    char *str;
    size_t size;
} respStr;

typedef struct {
    uint64_t value;
} respInt;

typedef struct {
    respTypes type;
    union {
        respStr str;
        respInt integer;
    } value;
} respObject;
#endif
