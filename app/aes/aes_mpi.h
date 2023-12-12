#ifndef PARALLEL_AND_DISTRIBUTED_PROGRAMMING_PROJECT_AES_MPI_H
#define PARALLEL_AND_DISTRIBUTED_PROGRAMMING_PROJECT_AES_MPI_H

#include "aes_common.h"

void initAES(int argc, char *argv[]);

void finalizeAES();

void keyExpansion(uint8_t *originalKey, uint8_t *expandedKey);

void aesEncrypt(uint8_t *original_block, size_t blocks, uint8_t *outputBlock, uint8_t *expandedKey);

void aesDecrypt(uint8_t *encrypted_block, size_t blocks, uint8_t *outputBlock, uint8_t *expandedKey);

void aesEncryptBlock(uint8_t *inputBlock, uint8_t *outputBlock, uint8_t *roundKeys);

void aesDecryptBlock(uint8_t *inputBlock, uint8_t *outputBlock, uint8_t *roundKeys);

#endif //PARALLEL_AND_DISTRIBUTED_PROGRAMMING_PROJECT_AES_MPI_H
