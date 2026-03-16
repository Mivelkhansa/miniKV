#include "resp.h"
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Returns array size (>0), 0 = need more data, -1 = invalid
int respRequestArrayLen(const char *buf, size_t bufUsed, size_t *byteLen) {
    size_t pos = 0;

    if (pos >= bufUsed || buf[pos] != '*') return -1;
    pos++;

    if (pos >= bufUsed) return 0;

    if (!isdigit(buf[pos])) return -1;
    unsigned int arraySize = 0;
    while (pos < bufUsed && isdigit(buf[pos])) {
        arraySize = arraySize * 10 + (buf[pos] - '0');
        pos++;
    }

    if (pos + 1 >= bufUsed) return 0;
    if (buf[pos] != '\r' || buf[pos + 1] != '\n') return -1;
    pos += 2;

    for (unsigned int i = 0; i < arraySize; i++) {
        if (pos >= bufUsed || buf[pos] != '$') return -1;
        pos++;

        int bulkLen = 0;
        if (pos >= bufUsed) return 0;
        while (pos < bufUsed && isdigit(buf[pos])) {
            bulkLen = bulkLen * 10 + (buf[pos] - '0');
            pos++;
        }

        if (pos + 1 >= bufUsed) return 0;
        if (buf[pos] != '\r' || buf[pos + 1] != '\n') return -1;
        pos += 2;

        if (pos + bulkLen + 2 > bufUsed) return 0; // <-- include the trailing \r\n
        pos += bulkLen + 2;
    }

    *byteLen = pos;
    return arraySize;
}

// Copy strings safely
respStr *getArgsFromRespArray(const char *buf, size_t bufLen, int arraySize) {
    size_t pos = 0;

    if (buf[pos] != '*') return NULL;
    pos++;
    while (pos < bufLen && isdigit(buf[pos])) pos++;
    pos += 2; // skip "\r\n"

    respStr *args = malloc(sizeof(respStr) * arraySize);
    if (!args) return NULL;

    for (int i = 0; i < arraySize; i++) {
        if (pos >= bufLen || buf[pos] != '$') { // safety
            free(args);
            return NULL;
        }
        pos++;
        int bulkLen = 0;
        while (pos < bufLen && isdigit(buf[pos])) {
            bulkLen = bulkLen * 10 + (buf[pos] - '0');
            pos++;
        }
        pos += 2; // skip "\r\n"

        if (pos + bulkLen + 2 > bufLen) { // check full bulk string
            free(args);
            return NULL;
        }

        args[i].str = malloc(bulkLen + 1);
        memcpy(args[i].str, buf + pos, bulkLen);
        args[i].str[bulkLen] = '\0'; // null terminate
        args[i].size = bulkLen;

        pos += bulkLen + 2; // skip bulk + \r\n
    }

    return args;
}
void freeRespArgs(respStr *args, int arraySize) {
    if (!args) return;
    for (int i = 0; i < arraySize; i++) free(args[i].str);
    free(args);
}

// Moves buffer to next potential RESP message
int clearBufferFromInvalidRespRequest(char *buf, size_t bufUsed) {
    for (size_t i = 1; i < bufUsed; i++) {
        if (buf[i] == '*') {
            memmove(buf, buf + i, bufUsed - i);
            return bufUsed - i;
        }
    }
    return 0;
}

// Safe compare
bool respStrCmpWithCstr(respStr *a, const char *b) {
    if (a->size != strlen(b)) return false;
    for (size_t i = 0; i < a->size; i++) {
        if (tolower((unsigned char)a->str[i]) != tolower((unsigned char)b[i]))
            return false;
    }
    return true;
}
