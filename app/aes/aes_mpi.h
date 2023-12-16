#ifndef PARALLEL_AND_DISTRIBUTED_PROGRAMMING_PROJECT_AES_MPI_H
#define PARALLEL_AND_DISTRIBUTED_PROGRAMMING_PROJECT_AES_MPI_H

#include "aes_common.h"

void addRoundKey(uint8_t *state, uint8_t *roundKeyMatrix, uint8_t roundNumber);

void mixColumns(uint8_t *state);

void invMixColumns(uint8_t *state);

void shiftRows(uint8_t *state);

void invShiftRows(uint8_t *state);

void subBytes(uint8_t *state);

void invSubBytes(uint8_t *state);

#endif //PARALLEL_AND_DISTRIBUTED_PROGRAMMING_PROJECT_AES_MPI_H
