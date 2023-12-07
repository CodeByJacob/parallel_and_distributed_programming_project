#ifndef PARALLEL_AND_DISTRIBUTED_PROGRAMMING_PROJECT_AES_MPI_H
#define PARALLEL_AND_DISTRIBUTED_PROGRAMMING_PROJECT_AES_MPI_H

#include "aes_common.h"

void aesEncrypt(uint8_t *inputBlock, uint8_t *outputBlock, uint8_t *roundKeys);

void aesDecrypt(uint8_t *inputBlock, uint8_t *outputBlock, uint8_t *roundKeys);

#endif //PARALLEL_AND_DISTRIBUTED_PROGRAMMING_PROJECT_AES_MPI_H
