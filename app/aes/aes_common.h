#ifndef PARALLEL_AND_DISTRIBUTED_PROGRAMMING_PROJECT_AES_COMMON_H
#define PARALLEL_AND_DISTRIBUTED_PROGRAMMING_PROJECT_AES_COMMON_H

#include <stdint-gcc.h>

void AES_init();

void AES_encrypt(uint8_t *block, uint8_t *key);

void AES_decrypt(uint8_t *block, uint8_t *key);

#endif //PARALLEL_AND_DISTRIBUTED_PROGRAMMING_PROJECT_AES_COMMON_H
