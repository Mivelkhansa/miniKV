/*Copyright 2026 mivel khansa

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "dict.h"
#include "resp.h"

dictEntry *makeDictEntry(respStr key, respStr value) {
    dictEntry *entry = malloc(sizeof(dictEntry));
    if (!entry) return NULL;
    entry->key = key;
    entry->value = value;
    entry->next = NULL;
    return entry;
}

respStr *makeRespStr(char *str, int len) {
    respStr *resp = malloc(sizeof(respStr));
    if (!resp) return NULL;
    resp->str = str;
    resp->size = len;
    return resp;
}



void freeRespStr(respStr *resp) {
    if (!resp) return;
    free(resp->str);
    free(resp);
}
