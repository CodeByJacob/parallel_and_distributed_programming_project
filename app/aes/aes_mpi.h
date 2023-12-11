#ifndef PARALLEL_AND_DISTRIBUTED_PROGRAMMING_PROJECT_AES_MPI_H
#define PARALLEL_AND_DISTRIBUTED_PROGRAMMING_PROJECT_AES_MPI_H

#include "aes_common.h"

void initAES(int argc, char *argv[]);

void finalizeAES();

void keyExpansion(uint8_t *originalKey, uint8_t *expandedKey);

void aesEncrypt(uint8_t *inputBlock, uint8_t *outputBlock, uint8_t *roundKeys, size_t blockSize);

void aesDecrypt(uint8_t *inputBlock, uint8_t *outputBlock, uint8_t *roundKeys, size_t blockSize);

#endif //PARALLEL_AND_DISTRIBUTED_PROGRAMMING_PROJECT_AES_MPI_H
