#ifndef PROJECT_TEST_HELPER_H
#define PROJECT_TEST_HELPER_H

#define BLOCK_SIZE 16

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    const char* path;
    char* content;
    size_t size;
} FileData;

typedef struct {
    size_t size;
    uint8_t* data;
} ConvertedData;

FileData readFromFile(const char *filename);

char *generateRandomKey(size_t keyLengthInBits);

char **divideIntoBlocks(const char *input, size_t *blockCount);
ConvertedData convertDataToUint8(const char *data);
void freeUint8Array(uint8_t *array);
void freeBlocks(char **blocks, size_t blockCount);


#endif //PROJECT_TEST_HELPER_H
