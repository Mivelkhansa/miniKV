#ifndef PARSER_H
#define PARSER_H
#include <stddef.h>
#include <stdbool.h>
#include "resp.h"

int respRequestArrayLen(const char *buf, size_t bufUsed, size_t *byteLen);
int clearBufferFromInvalidRespRequest(char *buf, size_t bufUsed);
respStr *getArgsFromRespArray(const char *buf, size_t bufLen, int arraySize);
void freeRespArgs(respStr *args, int arraySize);
bool respStrCmpWithCstr(respStr *a, const char *b);


#endif
