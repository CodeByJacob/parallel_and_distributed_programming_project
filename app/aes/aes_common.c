#include "aes_common.h"

//uint8_t getSBoxValue(uint8_t value) {
//    return sBox[value / 16][value % 16];
//}
//

//uint8_t getInvSBoxValue(uint8_t value) {
//    uint8_t _value = getSBoxValue(value);
//
//    return invSBox[_value / 16][_value % 16];
//}

uint8_t gmult(uint8_t a, uint8_t b) {
    uint8_t product = 0;

    for (int i = 0; i < 8; i++) {
        if (b & 1) {
            product ^= a;
        }

        uint8_t highBitSet = a & 0x80;
        a <<= 1;

        if (highBitSet) {
            a ^= 0x1b;
        }

        b >>= 1;
    }

    return product;
}

void coef_addition(uint8_t a[], uint8_t b[], uint8_t result[]) {
    result[0] = a[0] ^ b[0];
    result[1] = a[1] ^ b[1];
    result[2] = a[2] ^ b[2];
    result[3] = a[3] ^ b[3];
}

void coef_multiplication(uint8_t *a, uint8_t *b, uint8_t *result) {
    result[0] = gmult(a[0], b[0]) ^ gmult(a[3], b[1]) ^ gmult(a[2], b[2]) ^ gmult(a[1], b[3]);
    result[1] = gmult(a[1], b[0]) ^ gmult(a[0], b[1]) ^ gmult(a[3], b[2]) ^ gmult(a[2], b[3]);
    result[2] = gmult(a[2], b[0]) ^ gmult(a[1], b[1]) ^ gmult(a[0], b[2]) ^ gmult(a[3], b[3]);
    result[3] = gmult(a[3], b[0]) ^ gmult(a[2], b[1]) ^ gmult(a[1], b[2]) ^ gmult(a[0], b[3]);
}

int cipherKey;

int numColumns = 4; // Number of columns (32-bit words) comprising the State
int numKeyWords;   // Number of 32-bit words comprising the Cipher Key
int numRounds;     // Number of rounds, a function of numKeyWords and numColumns

uint8_t *getRoundConstant(uint8_t roundNumber) {
    if (roundNumber == 1) {
        roundConstants[0] = 0x01;
    } else if (roundNumber > 1) {
        roundConstants[0] = 0x02;
        roundNumber--;
        while (roundNumber > 1) {
            roundConstants[0] = gmult(roundConstants[0], 0x02);
            roundNumber--;
        }
    }
    return roundConstants;
}

void addRoundKey(uint8_t *state, uint8_t *roundKeyMatrix, uint8_t roundNumber) {
    uint8_t column;

    for (column = 0; column < numColumns; column++) {
        state[numColumns * 0 + column] ^= roundKeyMatrix[4 * numColumns * roundNumber + 4 * column + 0];
        state[numColumns * 1 + column] ^= roundKeyMatrix[4 * numColumns * roundNumber + 4 * column + 1];
        state[numColumns * 2 + column] ^= roundKeyMatrix[4 * numColumns * roundNumber + 4 * column + 2];
        state[numColumns * 3 + column] ^= roundKeyMatrix[4 * numColumns * roundNumber + 4 * column + 3];
    }
}

void mixColumns(uint8_t *state) {

    uint8_t mixMatrix[] = {0x02, 0x01, 0x01, 0x03}; // MixColumns matrix
    uint8_t row, column, inputColumn[4], resultColumn[4];

    for (column = 0; column < numColumns; column++) {
        for (row = 0; row < 4; row++) {
            inputColumn[row] = state[numColumns * row + column];
        }

        coef_multiplication(mixMatrix, inputColumn, resultColumn);

        for (row = 0; row < 4; row++) {
            state[numColumns * row + column] = resultColumn[row];
        }
    }
}

void invMixColumns(uint8_t *state) {

    uint8_t inverseMixColumnMatrix[] = {0x0e, 0x09, 0x0d, 0x0b};

    uint8_t column[4], result[4];

    for (uint8_t j = 0; j < numColumns; j++) {
        for (uint8_t i = 0; i < 4; i++) {
            column[i] = state[numColumns * i + j];
        }

        coef_multiplication(inverseMixColumnMatrix, column, result);

        for (int i = 0; i < 4; i++) {
            state[numColumns * i + j] = result[i];
        }
    }
}

void shiftRows(uint8_t *state) {

    uint8_t row, shiftAmount, col, tmp;

    for (row = 1; row < 4; row++) {
        shiftAmount = row;  // shift(1,4)=1; shift(2,4)=2; shift(3,4)=3

        for (col = 0; col < numColumns; col++) {
            tmp = state[numColumns * row + 0];

            for (uint8_t k = 1; k < numColumns; k++) {
                state[numColumns * row + k - 1] = state[numColumns * row + k];
            }

            state[numColumns * row + numColumns - 1] = tmp;
            shiftAmount--;
            if (shiftAmount == 0) {
                break;  // No need to shift further
            }
        }
    }
}

void invShiftRows(uint8_t *state) {

    uint8_t row, shiftAmount, col, tmp;

    for (row = 1; row < 4; row++) {
        shiftAmount = row;

        for (col = numColumns - 1; col > 0; col--) {
            tmp = state[numColumns * row + numColumns - 1];

            for (uint8_t k = numColumns - 1; k > 0; k--) {
                state[numColumns * row + k] = state[numColumns * row + k - 1];
            }

            state[numColumns * row + 0] = tmp;
            shiftAmount--;
            if (shiftAmount == 0) {
                break;
            }
        }
    }
}

void subBytes(uint8_t *state) {

    for (uint8_t row = 0; row < 4; row++) {
        for (uint8_t col = 0; col < numColumns; col++) {
//            state[numColumns * row + col] = getSBoxValue(state[numColumns * row + col]);
            state[numColumns * row + col] = sBox[state[numColumns * row + col]];
        }
    }
}

void invSubBytes(uint8_t *state) {

    for (uint8_t row = 0; row < 4; row++) {
        for (uint8_t col= 0; col < numColumns; col++) {
//            state[numColumns*row+col] = getInvSBoxValue(state[numColumns*row+col]);
            state[numColumns*row+col] = invSBox[state[numColumns*row+col]];
        }
    }
}

void subWord(uint8_t *word) {

    for (uint8_t i = 0; i < 4; i++) {
//        word[i] = getSBoxValue(word[i]);
        word[i] = sBox[word[i]];
    }
}


void rotWord(uint8_t *word) {

    uint8_t tmp = word[0];

    for (uint8_t i = 0; i < 3; i++) {
        word[i] = word[i + 1];
    }

    word[3] = tmp;
}

void keyExpansion(uint8_t *originalKey, uint8_t *expandedKey) {

    uint8_t tempWord[4];
    uint8_t length = numColumns * (numRounds + 1);

    // Copy the original key to the first part of the expanded key
    for (uint8_t i = 0; i < numKeyWords; i++) {
        expandedKey[4 * i + 0] = originalKey[4 * i + 0];
        expandedKey[4 * i + 1] = originalKey[4 * i + 1];
        expandedKey[4 * i + 2] = originalKey[4 * i + 2];
        expandedKey[4 * i + 3] = originalKey[4 * i + 3];
    }

    // Generate the remaining part of the expanded key
    for (uint8_t i = numKeyWords; i < length; i++) {
        tempWord[0] = expandedKey[4 * (i - 1) + 0];
        tempWord[1] = expandedKey[4 * (i - 1) + 1];
        tempWord[2] = expandedKey[4 * (i - 1) + 2];
        tempWord[3] = expandedKey[4 * (i - 1) + 3];

        if (i % numKeyWords == 0) {
            rotWord(tempWord);
            subWord(tempWord);
            coef_addition(tempWord, getRoundConstant(i / numKeyWords), tempWord);
        } else if (numKeyWords > 6 && i % numKeyWords == 4) {
            subWord(tempWord);
        }

        expandedKey[4 * i + 0] = expandedKey[4 * (i - numKeyWords) + 0] ^ tempWord[0];
        expandedKey[4 * i + 1] = expandedKey[4 * (i - numKeyWords) + 1] ^ tempWord[1];
        expandedKey[4 * i + 2] = expandedKey[4 * (i - numKeyWords) + 2] ^ tempWord[2];
        expandedKey[4 * i + 3] = expandedKey[4 * (i - numKeyWords) + 3] ^ tempWord[3];
    }
}

uint8_t *initializeAES(size_t keySize) {
    switch (keySize) {
        default:
        case 16: numKeyWords = 4; numRounds = 10; break;
        case 24: numKeyWords = 6; numRounds = 12; break;
        case 32: numKeyWords = 8; numRounds = 14; break;
    }

    return malloc(numColumns * (numRounds + 1) * 4);
}

void aesEncrypt(uint8_t *inputBlock, uint8_t *outputBlock, uint8_t *roundKeys) {

    uint8_t state[4 * numColumns];
    uint8_t round, i, j;

    // Copy input block to the state array
    for (i = 0; i < 4; i++) {
        for (j = 0; j < numColumns; j++) {
            state[numColumns * i + j] = inputBlock[i + 4 * j];
        }
    }

    // Initial round key addition
    addRoundKey(state, roundKeys, 0);

    // Main rounds
    for (round = 1; round < numRounds; round++) {
        subBytes(state);
        shiftRows(state);
        mixColumns(state);
        addRoundKey(state, roundKeys, round);
    }

    // Final round
    subBytes(state);
    shiftRows(state);
    addRoundKey(state, roundKeys, numRounds);

    // Copy the state array to the output block
    for (i = 0; i < 4; i++) {
        for (j = 0; j < numColumns; j++) {
            outputBlock[i + 4 * j] = state[numColumns * i + j];
        }
    }
}

void aesDecrypt(uint8_t *inputBlock, uint8_t *outputBlock, uint8_t *roundKeys) {

    uint8_t state[4 * numColumns];
    uint8_t round, i, j;

    // Copy input block to the state array
    for (i = 0; i < 4; i++) {
        for (j = 0; j < numColumns; j++) {
            state[numColumns * i + j] = inputBlock[i + 4 * j];
        }
    }

    // Initial round key addition
    addRoundKey(state, roundKeys, numRounds);

    // Main rounds in reverse order
    for (round = numRounds - 1; round >= 1; round--) {
        invShiftRows(state);
        invSubBytes(state);
        addRoundKey(state, roundKeys, round);
        invMixColumns(state);
    }

    // Final round
    invShiftRows(state);
    invSubBytes(state);
    addRoundKey(state, roundKeys, 0);

    // Copy the state array to the output block
    for (i = 0; i < 4; i++) {
        for (j = 0; j < numColumns; j++) {
            outputBlock[i + 4 * j] = state[numColumns * i + j];
        }
    }
}