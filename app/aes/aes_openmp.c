#include <assert.h>
#include "aes_openmp.h"

void addRoundKey(uint8_t *state, uint8_t *roundKeyMatrix, uint8_t roundNumber) {
    for (uint8_t column = 0; column < AES_NUM_OF_COLUMNS; column++) {
        for (uint8_t row = 0; row < 4; row++) {
            state[AES_NUM_OF_COLUMNS * row + column] ^= roundKeyMatrix[4 * AES_NUM_OF_COLUMNS * roundNumber + 4 * column + row];
        }
    }
}

void mixColumns(uint8_t *state) {
    uint8_t mixMatrix[] = {0x02, 0x01, 0x01, 0x03};
    uint8_t inputColumn[4], resultColumn[4];

    for (uint8_t column = 0; column < AES_NUM_OF_COLUMNS; column++) {
        for (uint8_t row = 0; row < 4; row++) {
            inputColumn[row] = state[AES_NUM_OF_COLUMNS * row + column];
        }

        coef_multiplication(mixMatrix, inputColumn, resultColumn);

        for (uint8_t row = 0; row < 4; row++) {
            state[AES_NUM_OF_COLUMNS * row + column] = resultColumn[row];
        }
    }
}

void invMixColumns(uint8_t *state) {
    uint8_t inverseMixColumnMatrix[] = {0x0e, 0x09, 0x0d, 0x0b};

    uint8_t column[4], result[4];

    for (uint8_t j = 0; j < AES_NUM_OF_COLUMNS; j++) {
        for (uint8_t i = 0; i < 4; i++) {
            column[i] = state[AES_NUM_OF_COLUMNS * i + j];
        }

        coef_multiplication(inverseMixColumnMatrix, column, result);

        for (uint8_t i = 0; i < 4; i++) {
            state[AES_NUM_OF_COLUMNS * i + j] = result[i];
        }
    }
}

void shiftRows(uint8_t *state) {
    uint8_t shiftAmount, tmp;

    for (uint8_t row = 1; row < 4; row++) {
        shiftAmount = row;

        for (uint8_t col = 0; col < AES_NUM_OF_COLUMNS; col++) {
            tmp = state[AES_NUM_OF_COLUMNS * row + 0];

            for (uint8_t k = 1; k < AES_NUM_OF_COLUMNS; k++) {
                state[AES_NUM_OF_COLUMNS * row + k - 1] = state[AES_NUM_OF_COLUMNS * row + k];
            }

            state[AES_NUM_OF_COLUMNS * row + AES_NUM_OF_COLUMNS - 1] = tmp;
            shiftAmount--;
            if (shiftAmount == 0) {
                break;
            }
        }
    }
}

void invShiftRows(uint8_t *state) {
    uint8_t shiftAmount, tmp;

    for (uint8_t row = 1; row < 4; row++) {
        shiftAmount = row;

        for (uint8_t col = AES_NUM_OF_COLUMNS - 1; col > 0; col--) {
            tmp = state[AES_NUM_OF_COLUMNS * row + AES_NUM_OF_COLUMNS - 1];

            for (uint8_t k = AES_NUM_OF_COLUMNS - 1; k > 0; k--) {
                state[AES_NUM_OF_COLUMNS * row + k] = state[AES_NUM_OF_COLUMNS * row + k - 1];
            }

            state[AES_NUM_OF_COLUMNS * row + 0] = tmp;
            shiftAmount--;
            if (shiftAmount == 0) {
                break;
            }
        }
    }
}

void subBytes(uint8_t *state) {

    for (uint8_t row = 0; row < 4; row++) {
        for (uint8_t col = 0; col < AES_NUM_OF_COLUMNS; col++) {
            state[AES_NUM_OF_COLUMNS * row + col] = sBox[state[AES_NUM_OF_COLUMNS * row + col]];
        }
    }
}

void invSubBytes(uint8_t *state) {

    for (uint8_t row = 0; row < 4; row++) {
        for (uint8_t col= 0; col < AES_NUM_OF_COLUMNS; col++) {
            state[AES_NUM_OF_COLUMNS*row+col] = invSBox[state[AES_NUM_OF_COLUMNS*row+col]];
        }
    }
}

void keyExpansion(uint8_t *originalKey, uint8_t *expandedKey) {
    uint8_t tempWord[4];
    uint8_t length = AES_NUM_OF_COLUMNS * (AES_NUM_OF_ROUNDS + 1);

    for (uint8_t i = 0; i < AES_KEYWORDS; i++) {
        for (uint8_t j = 0; j < 4; j++) {
            expandedKey[4 * i + j] = originalKey[4 * i + j];
        }
    }

    for (uint8_t i = AES_KEYWORDS; i < length; i++) {
        for (uint8_t j = 0; j < 4; j++) {
            tempWord[j] = expandedKey[4 * (i - 1) + j];
        }

        if (i % AES_KEYWORDS == 0) {
            rotWord(tempWord);
            subWord(tempWord);
            coef_addition(tempWord, getRoundConstant(i / AES_KEYWORDS), tempWord);
        } else if (AES_KEYWORDS > 6 && i % AES_KEYWORDS == 4) {
            subWord(tempWord);
        }

        for (uint8_t j = 0; j < 4; j++) {
            expandedKey[4 * i + j] = expandedKey[4 * (i - AES_KEYWORDS) + j] ^ tempWord[j];
        }
    }
}


void aesEncryptBlock(uint8_t *inputBlock, uint8_t *outputBlock, uint8_t *roundKeys) {
    uint8_t state[4 * AES_NUM_OF_COLUMNS];

    #pragma omp parallel for collapse(2) shared(state)
    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = 0; j < AES_NUM_OF_COLUMNS; j++) {
            state[AES_NUM_OF_COLUMNS * i + j] = inputBlock[i + 4 * j];
        }
    }

    addRoundKey(state, roundKeys, 0);

    #pragma omp parallel for shared(state)
    for (uint8_t round = 1; round < AES_NUM_OF_ROUNDS; round++) {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, roundKeys, round);
    }

    subBytes(state);
    shiftRows(state);
    addRoundKey(state, roundKeys, AES_NUM_OF_ROUNDS);

    #pragma omp parallel for collapse(2) shared(outputBlock, state)
    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = 0; j < AES_NUM_OF_COLUMNS; j++) {
            outputBlock[i + 4 * j] = state[AES_NUM_OF_COLUMNS * i + j];
        }
    }
}

void aesDecryptBlock(uint8_t *inputBlock, uint8_t *outputBlock, uint8_t *roundKeys) {
    uint8_t state[4 * AES_NUM_OF_COLUMNS];

    #pragma omp parallel for collapse(2) shared(state)
    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = 0; j < AES_NUM_OF_COLUMNS; j++) {
            state[AES_NUM_OF_COLUMNS * i + j] = inputBlock[i + 4 * j];
        }
    }

    addRoundKey(state, roundKeys, AES_NUM_OF_ROUNDS);

    #pragma omp parallel for shared(state)
    for (uint8_t round = AES_NUM_OF_ROUNDS - 1; round >= 1; round--) {
        invShiftRows(state);
        invSubBytes(state);
        addRoundKey(state, roundKeys, round);
        invMixColumns(state);
    }

    invShiftRows(state);
    invSubBytes(state);
    addRoundKey(state, roundKeys, 0);

    #pragma omp parallel for collapse(2) shared(outputBlock, state)
    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = 0; j < AES_NUM_OF_COLUMNS; j++) {
            outputBlock[i + 4 * j] = state[AES_NUM_OF_COLUMNS * i + j];
        }
    }
}

void aesEncrypt(uint8_t *original_block, size_t blocks, uint8_t *outputBlock, uint8_t *expandedKey) {
    size_t numBlocks = (blocks + BLOCK_SIZE - 1) / BLOCK_SIZE;
    Block msg[numBlocks];
    CipherBlock e_msg[numBlocks];

    uint8_t paddingSize = BLOCK_SIZE - (blocks % BLOCK_SIZE);
    paddingSize = (paddingSize == 0) ? BLOCK_SIZE : paddingSize;

    #pragma omp parallel for
    for (size_t i = 0; i < numBlocks; i++) {
        #pragma omp parallel for
        for (size_t j = 0; j < BLOCK_SIZE; j++) {
            size_t index = (i * BLOCK_SIZE) + j;
            if (index < blocks) {
                msg[i].data[j] = original_block[index];
            } else {
                msg[i].data[j] = paddingSize;
            }
        }

        aesEncryptBlock(msg[i].data, e_msg[i].data, expandedKey);

        for (size_t j = 0; j < BLOCK_SIZE; j++) {
            outputBlock[(i * BLOCK_SIZE) + j] = e_msg[i].data[j];
        }
    }
}

void aesDecrypt(uint8_t *encrypted_block, size_t blocks, uint8_t *outputBlock, uint8_t *expandedKey){
    size_t numBlocks = (blocks + BLOCK_SIZE - 1) / BLOCK_SIZE;
    CipherBlock d_msg[numBlocks];

    #pragma omp parallel for
    for (size_t i = 0; i < numBlocks; i++) {
        aesDecryptBlock(&encrypted_block[i * BLOCK_SIZE], d_msg[i].data, expandedKey);

        if (i == numBlocks - 1) {
            uint8_t lastByte = d_msg[i].data[BLOCK_SIZE - 1];
            size_t paddingSize = (lastByte <= BLOCK_SIZE) ? lastByte : 0;
            for (size_t j = 0; j < BLOCK_SIZE - paddingSize; j++) {
                outputBlock[(i * BLOCK_SIZE) + j] = d_msg[i].data[j];
            }
        } else {
            for (size_t j = 0; j < BLOCK_SIZE; j++) {
                outputBlock[(i * BLOCK_SIZE) + j] = d_msg[i].data[j];
            }
        }
    }
}

void initAES(int argc, char *argv[]) {
    // Left empty intentionally
}

void finalizeAES() {
    // Left empty intentionally
}

void testOriginalAndDecryptedBlock(uint8_t *originalBlock, uint8_t *decryptedBlock, size_t blocks) {
    assert(memcmp(originalBlock, decryptedBlock, blocks) == 0);
}