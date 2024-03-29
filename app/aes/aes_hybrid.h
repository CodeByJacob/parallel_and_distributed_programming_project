#ifndef PARALLEL_AND_DISTRIBUTED_PROGRAMMING_PROJECT_AES_HYBRID_H
#define PARALLEL_AND_DISTRIBUTED_PROGRAMMING_PROJECT_AES_HYBRID_H

#include "aes_common.h"

void addRoundKey(uint8_t *state, uint8_t *roundKeyMatrix, uint8_t roundNumber);

void mixColumns(uint8_t *state);

void invMixColumns(uint8_t *state);

void shiftRows(uint8_t *state);

void invShiftRows(uint8_t *state);

void subBytes(uint8_t *state);

void invSubBytes(uint8_t *state);

void keyExpansion(uint8_t *originalKey, uint8_t *expandedKey);

void aesEncrypt(uint8_t *original_block, size_t blocks, uint8_t *outputBlock, uint8_t *expandedKey);

void aesDecrypt(uint8_t *encrypted_block, size_t blocks, uint8_t *outputBlock, uint8_t *expandedKey);

void aesEncryptBlock(uint8_t *inputBlock, uint8_t *outputBlock, uint8_t *roundKeys);

void aesDecryptBlock(uint8_t *inputBlock, uint8_t *outputBlock, uint8_t *roundKeys);

void initAES(int argc, char *argv[]);

void finalizeAES();

#endif //PARALLEL_AND_DISTRIBUTED_PROGRAMMING_PROJECT_AES_HYBRID_H
