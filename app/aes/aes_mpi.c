#include "aes_mpi.h"
#include <mpi.h>

void initAES(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
}
void finalizeAES() {
    MPI_Finalize();
}

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
        for (uint8_t j = 0; j < 4; j++){
            expandedKey[4 * i + j] = originalKey[4 * i + j];
        }
    }

    for (uint8_t i = AES_KEYWORDS; i < length; i++) {
        for (uint8_t j = 0; j < 4; j++){
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

    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = 0; j < AES_NUM_OF_COLUMNS; j++) {
            state[AES_NUM_OF_COLUMNS * i + j] = inputBlock[i + 4 * j];
        }
    }

    addRoundKey(state, roundKeys, 0);

    for (uint8_t round = 1; round < AES_NUM_OF_ROUNDS; round++) {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, roundKeys, round);
    }

    subBytes(state);
    shiftRows(state);
    addRoundKey(state, roundKeys, AES_NUM_OF_ROUNDS);

    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = 0; j < AES_NUM_OF_COLUMNS; j++) {
            outputBlock[i + 4 * j] = state[AES_NUM_OF_COLUMNS * i + j];
        }
    }
}

void aesDecryptBlock(uint8_t *inputBlock, uint8_t *outputBlock, uint8_t *roundKeys) {

    uint8_t state[4 * AES_NUM_OF_COLUMNS];

    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = 0; j < AES_NUM_OF_COLUMNS; j++) {
            state[AES_NUM_OF_COLUMNS * i + j] = inputBlock[i + 4 * j];
        }
    }

    addRoundKey(state, roundKeys, AES_NUM_OF_ROUNDS);

    for (uint8_t round = AES_NUM_OF_ROUNDS - 1; round >= 1; round--) {
        invShiftRows(state);
        invSubBytes(state);
        addRoundKey(state, roundKeys, round);
        invMixColumns(state);
    }

    invShiftRows(state);
    invSubBytes(state);
    addRoundKey(state, roundKeys, 0);

    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = 0; j < AES_NUM_OF_COLUMNS; j++) {
            outputBlock[i + 4 * j] = state[AES_NUM_OF_COLUMNS * i + j];
        }
    }
}

void aesSequentialEncrypt(uint8_t *original_block, size_t blocks, uint8_t *outputBlock, uint8_t *expandedKey) {
    size_t numBlocks = (blocks + BLOCK_SIZE - 1) / BLOCK_SIZE; // Round up to handle any partial blocks
    Block msg[numBlocks];
    CipherBlock e_msg[numBlocks];

    for (size_t i = 0; i < numBlocks; i++) {
        for (size_t j = 0; j < BLOCK_SIZE; j++) {
            size_t index = (i * BLOCK_SIZE) + j;
            msg[i].data[j] = (index < blocks) ? original_block[index] : 0; // Handle partial block
        }

        aesEncryptBlock(msg[i].data, e_msg[i].data, expandedKey);

        for (size_t j = 0; j < BLOCK_SIZE; j++) {
            size_t index = (i * BLOCK_SIZE) + j;
            if (index < blocks) {
                outputBlock[index] = e_msg[i].data[j];
            }
        }
    }
}

void aesSequentialDecrypt(uint8_t *encrypted_block, size_t blocks, uint8_t *outputBlock, uint8_t *expandedKey) {
    size_t numBlocks = (blocks + BLOCK_SIZE - 1) / BLOCK_SIZE; // Round up to handle any partial blocks
    Block msg[numBlocks];
    CipherBlock d_msg[numBlocks];

    for (size_t i = 0; i < numBlocks; i++) {
        for (size_t j = 0; j < BLOCK_SIZE; j++) {
            size_t index = (i * BLOCK_SIZE) + j;
            msg[i].data[j] = (index < blocks) ? encrypted_block[index] : 0; // Handle partial block
        }

        aesDecryptBlock(msg[i].data, d_msg[i].data, expandedKey);

        for (size_t j = 0; j < BLOCK_SIZE; j++) {
            size_t index = (i * BLOCK_SIZE) + j;
            if (index < blocks) {
                outputBlock[index] = d_msg[i].data[j];
            }
        }
    }
}

// TODO: Each process gets a block that's size can be divided by 16
void aesEncrypt(uint8_t *original_block, size_t blocks, uint8_t *outputBlock, uint8_t *expandedKey) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int *sendcounts = malloc(size * sizeof(int));
    int *displs = malloc(size * sizeof(int));

    // Calculate the base blockSize for each process as a multiple of BLOCK_SIZE
    size_t baseBlockSize = (blocks / size / BLOCK_SIZE) * BLOCK_SIZE;
    size_t totalDistributedBlocks = baseBlockSize * (size - 1);

    // The last process takes the remainder
    size_t lastProcessBlocks = blocks - totalDistributedBlocks;

    // Compute send counts and displacements
    for (int i = 0; i < size; ++i) {
        if (i < size - 1) {
            sendcounts[i] = baseBlockSize;
        } else {
            // Last process takes the remainder
            sendcounts[i] = lastProcessBlocks;
        }
        displs[i] = (i > 0) ? (displs[i - 1] + sendcounts[i - 1]) : 0;
    }

    // Local block size for this process
    size_t localBlockSize = sendcounts[rank];

    uint8_t *localInputBlock = malloc(localBlockSize * sizeof(uint8_t));
    uint8_t *localOutputBlock = malloc(localBlockSize * sizeof(uint8_t));

    MPI_Scatterv(original_block, sendcounts, displs, MPI_UINT8_T, localInputBlock, localBlockSize, MPI_UINT8_T, 0, MPI_COMM_WORLD);

    aesSequentialEncrypt(localInputBlock, localBlockSize, localOutputBlock, expandedKey);

    MPI_Gatherv(localOutputBlock, localBlockSize, MPI_UINT8_T, outputBlock, sendcounts, displs, MPI_UINT8_T, 0, MPI_COMM_WORLD);

    if(rank == 0) {
        printf("Encrypted block:\n");
        printUint8Array(outputBlock, blocks);
    }

    // Clean up
    free(localInputBlock);
    free(localOutputBlock);
    free(sendcounts);
    free(displs);
}

void aesDecrypt(uint8_t *encrypted_block, size_t blocks, uint8_t *outputBlock, uint8_t *expandedKey) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int *sendcounts = malloc(size * sizeof(int));
    int *displs = malloc(size * sizeof(int));


    // Calculate the base blockSize for each process as a multiple of BLOCK_SIZE
    size_t baseBlockSize = (blocks / size / BLOCK_SIZE) * BLOCK_SIZE;
    size_t totalDistributedBlocks = baseBlockSize * (size - 1);

    // The last process takes the remainder
    size_t lastProcessBlocks = blocks - totalDistributedBlocks;

    // Compute send counts and displacements
    for (int i = 0; i < size; ++i) {
        if (i < size - 1) {
            sendcounts[i] = baseBlockSize;
        } else {
            // Last process takes the remainder
            sendcounts[i] = lastProcessBlocks;
        }
        displs[i] = (i > 0) ? (displs[i - 1] + sendcounts[i - 1]) : 0;
    }

    // Local block size for this process
    size_t localBlockSize = sendcounts[rank];

    uint8_t *localInputBlock = malloc(localBlockSize * sizeof(uint8_t));
    uint8_t *localOutputBlock = malloc(localBlockSize * sizeof(uint8_t));

    MPI_Scatterv(encrypted_block, sendcounts, displs, MPI_UINT8_T, localInputBlock, localBlockSize, MPI_UINT8_T, 0, MPI_COMM_WORLD);

    aesSequentialDecrypt(localInputBlock, localBlockSize, localOutputBlock, expandedKey);

    MPI_Gatherv(localOutputBlock, localBlockSize, MPI_UINT8_T, outputBlock, sendcounts, displs, MPI_UINT8_T, 0, MPI_COMM_WORLD);

    // Clean up
    free(localInputBlock);
    free(localOutputBlock);
    free(sendcounts);
    free(displs);
}
