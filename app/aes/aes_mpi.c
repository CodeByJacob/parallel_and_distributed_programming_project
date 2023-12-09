#include "aes_mpi.h"
#include <mpi.h>

#define BLOCK_SIZE 16

void initAES(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
}

void finalizeAES() {
    MPI_Finalize();
}

void addRoundKey(uint8_t *state, uint8_t *roundKeyMatrix, uint8_t roundNumber) {
    uint8_t column;

    for (column = 0; column < AES_NUM_OF_COLUMNS; column++) {
        state[AES_NUM_OF_COLUMNS * 0 + column] ^= roundKeyMatrix[4 * AES_NUM_OF_COLUMNS * roundNumber + 4 * column + 0];
        state[AES_NUM_OF_COLUMNS * 1 + column] ^= roundKeyMatrix[4 * AES_NUM_OF_COLUMNS * roundNumber + 4 * column + 1];
        state[AES_NUM_OF_COLUMNS * 2 + column] ^= roundKeyMatrix[4 * AES_NUM_OF_COLUMNS * roundNumber + 4 * column + 2];
        state[AES_NUM_OF_COLUMNS * 3 + column] ^= roundKeyMatrix[4 * AES_NUM_OF_COLUMNS * roundNumber + 4 * column + 3];
    }
}

void mixColumns(uint8_t *state) {

    uint8_t mixMatrix[] = {0x02, 0x01, 0x01, 0x03};
    uint8_t row, column, inputColumn[4], resultColumn[4];

    for (column = 0; column < AES_NUM_OF_COLUMNS; column++) {
        for (row = 0; row < 4; row++) {
            inputColumn[row] = state[AES_NUM_OF_COLUMNS * row + column];
        }

        coef_multiplication(mixMatrix, inputColumn, resultColumn);

        for (row = 0; row < 4; row++) {
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

        for (int i = 0; i < 4; i++) {
            state[AES_NUM_OF_COLUMNS * i + j] = result[i];
        }
    }
}

void shiftRows(uint8_t *state) {

    uint8_t row, shiftAmount, col, tmp;

    for (row = 1; row < 4; row++) {
        shiftAmount = row;

        for (col = 0; col < AES_NUM_OF_COLUMNS; col++) {
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

    uint8_t row, shiftAmount, col, tmp;

    for (row = 1; row < 4; row++) {
        shiftAmount = row;

        for (col = AES_NUM_OF_COLUMNS - 1; col > 0; col--) {
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
        expandedKey[4 * i + 0] = originalKey[4 * i + 0];
        expandedKey[4 * i + 1] = originalKey[4 * i + 1];
        expandedKey[4 * i + 2] = originalKey[4 * i + 2];
        expandedKey[4 * i + 3] = originalKey[4 * i + 3];
    }

    for (uint8_t i = AES_KEYWORDS; i < length; i++) {
        tempWord[0] = expandedKey[4 * (i - 1) + 0];
        tempWord[1] = expandedKey[4 * (i - 1) + 1];
        tempWord[2] = expandedKey[4 * (i - 1) + 2];
        tempWord[3] = expandedKey[4 * (i - 1) + 3];

        if (i % AES_KEYWORDS == 0) {
            rotWord(tempWord);
            subWord(tempWord);
            coef_addition(tempWord, getRoundConstant(i / AES_KEYWORDS), tempWord);
        } else if (AES_KEYWORDS > 6 && i % AES_KEYWORDS == 4) {
            subWord(tempWord);
        }

        expandedKey[4 * i + 0] = expandedKey[4 * (i - AES_KEYWORDS) + 0] ^ tempWord[0];
        expandedKey[4 * i + 1] = expandedKey[4 * (i - AES_KEYWORDS) + 1] ^ tempWord[1];
        expandedKey[4 * i + 2] = expandedKey[4 * (i - AES_KEYWORDS) + 2] ^ tempWord[2];
        expandedKey[4 * i + 3] = expandedKey[4 * (i - AES_KEYWORDS) + 3] ^ tempWord[3];
    }
}

void aesSequentialEncrypt(uint8_t *inputBlock, uint8_t *outputBlock, uint8_t *roundKeys) {

    uint8_t state[4 * AES_NUM_OF_COLUMNS];
    uint8_t round, i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < AES_NUM_OF_COLUMNS; j++) {
            state[AES_NUM_OF_COLUMNS * i + j] = inputBlock[i + 4 * j];
        }
    }

    addRoundKey(state, roundKeys, 0);

    for (round = 1; round < AES_NUM_OF_ROUNDS; round++) {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, roundKeys, round);
    }

    subBytes(state);
    shiftRows(state);
    addRoundKey(state, roundKeys, AES_NUM_OF_ROUNDS);

    for (i = 0; i < 4; i++) {
        for (j = 0; j < AES_NUM_OF_COLUMNS; j++) {
            outputBlock[i + 4 * j] = state[AES_NUM_OF_COLUMNS * i + j];
        }
    }
}

void aesSequentialDecrypt(uint8_t *inputBlock, uint8_t *outputBlock, uint8_t *roundKeys) {

    uint8_t state[4 * AES_NUM_OF_COLUMNS];
    uint8_t round, i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < AES_NUM_OF_COLUMNS; j++) {
            state[AES_NUM_OF_COLUMNS * i + j] = inputBlock[i + 4 * j];
        }
    }

    addRoundKey(state, roundKeys, AES_NUM_OF_ROUNDS);

    for (round = AES_NUM_OF_ROUNDS - 1; round >= 1; round--) {
        invShiftRows(state);
        invSubBytes(state);
        addRoundKey(state, roundKeys, round);
        invMixColumns(state);
    }

    invShiftRows(state);
    invSubBytes(state);
    addRoundKey(state, roundKeys, 0);

    for (i = 0; i < 4; i++) {
        for (j = 0; j < AES_NUM_OF_COLUMNS; j++) {
            outputBlock[i + 4 * j] = state[AES_NUM_OF_COLUMNS * i + j];
        }
    }
}

void aesEncrypt(uint8_t *inputData, uint8_t *outputData, uint8_t *roundKeys, size_t blockSize) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    uint8_t localInputBlock[BLOCK_SIZE];
    uint8_t localOutputBlock[BLOCK_SIZE];

    MPI_Scatter(inputData, BLOCK_SIZE, MPI_UINT8_T, localInputBlock, BLOCK_SIZE, MPI_UINT8_T, 0, MPI_COMM_WORLD);

    aesSequentialEncrypt(localInputBlock, localOutputBlock, roundKeys);

    MPI_Gather(localOutputBlock, BLOCK_SIZE, MPI_UINT8_T, outputData, BLOCK_SIZE, MPI_UINT8_T, 0, MPI_COMM_WORLD);
}

void aesDecrypt(uint8_t *inputData, uint8_t *outputData, uint8_t *roundKeys, size_t blockSize) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    uint8_t localInputBlock[BLOCK_SIZE];
    uint8_t localOutputBlock[BLOCK_SIZE];

    MPI_Scatter(inputData, BLOCK_SIZE, MPI_UINT8_T, localInputBlock, BLOCK_SIZE, MPI_UINT8_T, 0, MPI_COMM_WORLD);

    aesSequentialDecrypt(localInputBlock, localOutputBlock, roundKeys);

    MPI_Gather(localOutputBlock, BLOCK_SIZE, MPI_UINT8_T, outputData, BLOCK_SIZE, MPI_UINT8_T, 0, MPI_COMM_WORLD);
}
