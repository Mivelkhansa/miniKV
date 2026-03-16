#ifndef RESP_HELPER_H
#define RESP_HELPER_H
#include <stdbool.h>
#include "dict.h"
#include "resp.h"

dictEntry *makeDictEntry(respStr key, respStr value);
respStr *makeRespStr(char *str, int len);
void freeRespStr(respStr *resp);

#endif
