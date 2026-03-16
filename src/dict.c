/*
Copyright 2026 mivel khansa

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dict.h"
#include "resp.h"
//jenkins hash algorithm
uint32_t hash(const char* str, size_t size) {
    uint32_t hash = 0;
    for (size_t i = 0; i < size; i ++) {
        hash += (unsigned char)str[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

int dictSet(dict *dict, respStr *key, respStr *value) {
    size_t dict_position = hash(key->str, key->size) % dict->size;
    dictEntry *curDict = dict->slots[dict_position];

    respStr *keyCopy = malloc(key->size);
    respStr *valueCopy = malloc(value->size);
    memcpy(keyCopy->str, key->str, key->size);
    memcpy(valueCopy->str, value->str, value->size);
    keyCopy->size = key->size;
    valueCopy->size = value->size;

    while (curDict != NULL) {
        if (curDict->key.size == key->size &&
            memcmp(curDict->key.str, key->str, key->size) == 0) {
            // Key exists, update value
            curDict->value = *valueCopy;  // shallow copy (pointer is reused)
            return 0;
        }
        curDict = curDict->next;
    }

    // 2️⃣ Key doesn't exist, create a new entry
    dictEntry *dictentry = malloc(sizeof(dictEntry));
    if (!dictentry) return -1;  // allocation failure

    dictentry->key = *keyCopy;       // shallow copy
    dictentry->value = *valueCopy;   // shallow copy
    dictentry->next = dict->slots[dict_position];

    dict->slots[dict_position] = dictentry;
    return 0;
}
int dictDelete(dict *dict, respStr *key) {
    size_t dict_position = hash(key->str, key->size) % dict->size;
    dictEntry *cur = dict->slots[dict_position];
    dictEntry *prev = NULL;

    while (cur != NULL) {
        // memcmp returns 0 if keys are equal
        if (memcmp(cur->key.str, key->str, key->size) == 0 && cur->key.size == key->size) {
            if (prev == NULL) {
                // Node is at the head
                dict->slots[dict_position] = cur->next;
            } else {
                // Node is in the middle or end
                prev->next = cur->next;
            }
            free(cur);  // free memory of the removed node
            return 1;
        }
        prev = cur;
        cur = cur->next;
    }
    return 0;
}

respStr *dictGet(dict *dict, respStr *key) {
    size_t dict_position = hash(key->str, key->size) % dict->size;
    dictEntry *curDict = dict->slots[dict_position];
    while (curDict != NULL) {
        if (memcmp(curDict->key.str, key->str, key->size) == 0  && curDict->key.size == key->size) {
            return &curDict->value;
        }
        curDict = curDict->next;
    }
    return NULL;
}

dict *dictCreate(size_t size) {
    dict *d = malloc(sizeof(dict));

    d->size = size;
    d->slots = calloc(size, sizeof(dictEntry*));

    return d;
}

dict *dictResize(dict *dictionary, size_t size) {
    dict *newDict = dictCreate(size);
    for (size_t i = 0; i < dictionary->size; i++ ) {
        dictEntry *curDict = dictionary->slots[i];
        while (curDict != NULL) {
            dictEntry *next = curDict->next;
            size_t newPos = hash(curDict->key.str, curDict->key.size) % newDict->size;

            curDict->next = newDict->slots[newPos];
            newDict->slots[newPos] = curDict;
            curDict = next;
        }
    }
    free(dictionary->slots);
    free(dictionary);
    return newDict;
}

void dictFree(dict *dict) {
    for (size_t i = 0; i < dict->size; i++) {
        dictEntry *curDict = dict->slots[i];
        while (curDict != NULL) {
            dictEntry *next = curDict->next;
            free(curDict);
            curDict = next;
        }
    }
    free(dict->slots);
    free(dict);
}
