#ifndef HASH_H
#define HASH_H
#include <stddef.h>
#include "resp.h"

typedef struct dictEntry {
    respStr key;
    respStr value;
    struct dictEntry *next;
}dictEntry ;

typedef struct {
    dictEntry **slots;
    size_t size;
} dict;

int dictSet(dict *dict, respStr *key, respStr *value);
int dictDelete(dict *dict, respStr *key);
respStr *dictGet(dict *dict, respStr *key);
dict *dictCreate(size_t size);
dict *dictResize(dict *dictionary, size_t size);
void dictFree(dict *dict);

#endif
