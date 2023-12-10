#ifndef PROJECT_TEST_HELPER_H
#define PROJECT_TEST_HELPER_H

#define BLOCK_SIZE 16

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define AES_KEYSIZE 24

typedef struct {
    const char* path;
    char* content;
    size_t size;
} FileData;

typedef struct {
    size_t size;
    uint8_t* data;
} ConvertedData;

typedef struct {
    short num;
    size_t size;
    uint8_t data[BLOCK_SIZE];
} Block;

typedef struct {
    uint8_t data[AES_KEYSIZE];
} EncryptedBlock;

typedef struct {
    uint8_t data[AES_KEYSIZE];
} DecryptedBlock;

FileData readFromFile(const char *filename);

char *generateRandomKey(size_t keyLengthInBits);

char **divideIntoBlocks(const char *input, size_t *blockCount);
ConvertedData convertDataToUint8(const char *data);
void freeUint8Array(uint8_t *array);
void freeBlocks(char **blocks, size_t blockCount);
void shuffle(char *array, size_t n);


#endif //PROJECT_TEST_HELPER_H
