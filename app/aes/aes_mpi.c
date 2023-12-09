#include "aes_mpi.h"
#include <mpi.h>

void initAES(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
}

void finalizeAES() {
    MPI_Finalize();
}

void addRoundKey(uint8_t *state, uint8_t *roundKeyMatrix, uint8_t roundNumber, uint8_t numberOfColumns) {
    for (uint8_t column = 0; column < numberOfColumns; column++) {
        for(uint8_t row = 0; row < 4; row++) {
            state[numberOfColumns * row + column] ^= roundKeyMatrix[4 * numberOfColumns * roundNumber + 4 * column + row];
        }
    }
}

void mixColumns(uint8_t *state, uint8_t numberOfColumns) {

    uint8_t mixMatrix[] = {0x02, 0x01, 0x01, 0x03};
    uint8_t row, column, inputColumn[4], resultColumn[4];

    for (column = 0; column < numberOfColumns; column++) {
        for (row = 0; row < 4; row++) {
            inputColumn[row] = state[numberOfColumns * row + column];
        }

        coef_multiplication(mixMatrix, inputColumn, resultColumn);

        for (row = 0; row < 4; row++) {
            state[numberOfColumns * row + column] = resultColumn[row];
        }
    }
}

void invMixColumns(uint8_t *state, uint8_t numberOfColumns) {

    uint8_t inverseMixColumnMatrix[] = {0x0e, 0x09, 0x0d, 0x0b};

    uint8_t column[4], result[4];

    for (uint8_t j = 0; j < numberOfColumns; j++) {
        for (uint8_t i = 0; i < 4; i++) {
            column[i] = state[numberOfColumns * i + j];
        }

        coef_multiplication(inverseMixColumnMatrix, column, result);

        for (int i = 0; i < 4; i++) {
            state[numberOfColumns * i + j] = result[i];
        }
    }
}

void shiftRows(uint8_t *state, uint8_t numberOfColumns) {

    uint8_t row, shiftAmount, col, tmp;

    for (row = 1; row < 4; row++) {
        shiftAmount = row;

        for (col = 0; col < numberOfColumns; col++) {
            tmp = state[numberOfColumns * row + 0];

            for (uint8_t k = 1; k < numberOfColumns; k++) {
                state[numberOfColumns * row + k - 1] = state[numberOfColumns * row + k];
            }

            state[numberOfColumns * row + numberOfColumns - 1] = tmp;
            shiftAmount--;
            if (shiftAmount == 0) {
                break;
            }
        }
    }
}

void invShiftRows(uint8_t *state, uint8_t numberOfColumns) {

    uint8_t row, shiftAmount, col, tmp;

    for (row = 1; row < 4; row++) {
        shiftAmount = row;

        for (col = numberOfColumns - 1; col > 0; col--) {
            tmp = state[numberOfColumns * row + numberOfColumns - 1];

            for (uint8_t k = numberOfColumns - 1; k > 0; k--) {
                state[numberOfColumns * row + k] = state[numberOfColumns * row + k - 1];
            }

            state[numberOfColumns * row + 0] = tmp;
            shiftAmount--;
            if (shiftAmount == 0) {
                break;
            }
        }
    }
}

void subBytes(uint8_t *state, uint8_t numberOfColumns) {
    for (uint8_t row = 0; row < 4; row++) {
        for (uint8_t col = 0; col < numberOfColumns; col++) {
            state[numberOfColumns * row + col] = sBox[state[numberOfColumns * row + col]];
        }
    }
}

void invSubBytes(uint8_t *state, uint8_t numberOfColumns) {
    for (uint8_t row = 0; row < 4; row++) {
        for (uint8_t col= 0; col < numberOfColumns; col++) {
            state[numberOfColumns*row+col] = invSBox[state[numberOfColumns*row+col]];
        }
    }
}

void keyExpansion(uint8_t *originalKey, uint8_t *expandedKey, size_t blockSize) {
    uint8_t numberOfColumns = blockSize / 4;

    uint8_t tempWord[4];
    uint8_t length = numberOfColumns * (AES_NUM_OF_ROUNDS + 1);

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

void aesSequentialEncrypt(uint8_t *inputBlock, uint8_t *outputBlock, uint8_t *roundKeys, size_t blockSize) {
    uint8_t numberOfColumns = blockSize / 4;

    uint8_t state[4 * numberOfColumns];
    uint8_t round, i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < numberOfColumns; j++) {
            state[numberOfColumns * i + j] = inputBlock[i + 4 * j];
        }
    }

    addRoundKey(state, roundKeys, 0, numberOfColumns);

    for (round = 1; round < AES_NUM_OF_ROUNDS; round++) {
        subBytes(state, numberOfColumns);
        shiftRows(state, numberOfColumns);
        mixColumns(state, numberOfColumns);
        addRoundKey(state, roundKeys, round, numberOfColumns);
    }

    subBytes(state, numberOfColumns);
    shiftRows(state, numberOfColumns);
    addRoundKey(state, roundKeys, AES_NUM_OF_ROUNDS, numberOfColumns);

    for (i = 0; i < 4; i++) {
        for (j = 0; j < numberOfColumns; j++) {
            outputBlock[i + 4 * j] = state[numberOfColumns * i + j];
        }
    }
}

void aesSequentialDecrypt(uint8_t *inputBlock, uint8_t *outputBlock, uint8_t *roundKeys, size_t blockSize) {
    uint8_t numberOfColumns = blockSize / 4;

    uint8_t *state = malloc(4 * numberOfColumns * sizeof(uint8_t));
    uint8_t round, i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < numberOfColumns; j++) {
            state[numberOfColumns * i + j] = inputBlock[i + 4 * j];
        }
    }

    addRoundKey(state, roundKeys, AES_NUM_OF_ROUNDS, numberOfColumns);

    for (round = AES_NUM_OF_ROUNDS - 1; round >= 1; round--) {
        invShiftRows(state, numberOfColumns);
        invSubBytes(state, numberOfColumns);
        addRoundKey(state, roundKeys, round, numberOfColumns);
        invMixColumns(state, numberOfColumns);
    }

    invShiftRows(state, numberOfColumns);
    invSubBytes(state, numberOfColumns);
    addRoundKey(state, roundKeys, 0, numberOfColumns);

    for (i = 0; i < 4; i++) {
        for (j = 0; j < numberOfColumns; j++) {
            outputBlock[i + 4 * j] = state[numberOfColumns * i + j];
        }
    }

    free(state);
}


void aesEncrypt(uint8_t *inputData, uint8_t *outputData, uint8_t *roundKeys, size_t dataSize) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    size_t blockSize = dataSize / size;

    uint8_t localInputBlock[blockSize];
    uint8_t localOutputBlock[blockSize];

    MPI_Scatter(inputData, blockSize, MPI_UINT8_T, localInputBlock, blockSize, MPI_UINT8_T, 0, MPI_COMM_WORLD);

    aesSequentialEncrypt(localInputBlock, localOutputBlock, roundKeys, blockSize);

    MPI_Gather(localOutputBlock, blockSize, MPI_UINT8_T, outputData, blockSize, MPI_UINT8_T, 0, MPI_COMM_WORLD);
}

void aesDecrypt(uint8_t *inputData, uint8_t *outputData, uint8_t *roundKeys, size_t dataSize) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    size_t blockSize = dataSize / size;

    uint8_t localInputBlock[blockSize];
    uint8_t localOutputBlock[blockSize];

    MPI_Scatter(inputData, blockSize, MPI_UINT8_T, localInputBlock, blockSize, MPI_UINT8_T, 0, MPI_COMM_WORLD);

    aesSequentialDecrypt(localInputBlock, localOutputBlock, roundKeys, blockSize);

    MPI_Gather(localOutputBlock, blockSize, MPI_UINT8_T, outputData, blockSize, MPI_UINT8_T, 0, MPI_COMM_WORLD);
}
