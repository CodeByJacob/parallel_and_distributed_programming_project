#ifndef PARALLEL_AND_DISTRIBUTED_PROGRAMMING_PROJECT_AES_HYBRID_H
#define PARALLEL_AND_DISTRIBUTED_PROGRAMMING_PROJECT_AES_HYBRID_H

#include "aes_common.h"

void addRoundKey(uint8_t *state, uint8_t *roundKeyMatrix, uint8_t roundNumber, uint8_t numberOfColumns);

void mixColumns(uint8_t *state, uint8_t numberOfColumns);

void invMixColumns(uint8_t *state, uint8_t numberOfColumns);

void shiftRows(uint8_t *state, uint8_t numberOfColumns);

void invShiftRows(uint8_t *state, uint8_t numberOfColumns);

void subBytes(uint8_t *state, uint8_t numberOfColumns);

void invSubBytes(uint8_t *state, uint8_t numberOfColumns);

void keyExpansion(uint8_t *originalKey, uint8_t *expandedKey, size_t blockSize);

void aesEncrypt(uint8_t *inputBlock, uint8_t *outputBlock, uint8_t *roundKeys, size_t blockSize);

void aesDecrypt(uint8_t *inputBlock, uint8_t *outputBlock, uint8_t *roundKeys, size_t blockSize);

void initAES(int argc, char *argv[]);

void finalizeAES();

#endif //PARALLEL_AND_DISTRIBUTED_PROGRAMMING_PROJECT_AES_HYBRID_H
