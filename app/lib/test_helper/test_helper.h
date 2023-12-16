#ifndef PROJECT_TEST_HELPER_H
#define PROJECT_TEST_HELPER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

typedef struct {
    char *content;
    size_t size;
} FileData;

typedef struct {
    size_t size;
    uint8_t *data;
} ConvertedData;

typedef struct {
    char *msg_path;
    char *key_path;
} TestCase;

FileData readFromFile(const char *filename);

ConvertedData convertDataToUint8(const char *data);

void shuffle(char *array, size_t n);

char *generateRandomKey(size_t keyLengthInBits);


#endif //PROJECT_TEST_HELPER_H
