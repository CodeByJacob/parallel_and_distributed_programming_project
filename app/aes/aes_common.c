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
//            state[numColumns * row + col] = getSBoxValue(state[numColumns * row + col]);
            state[AES_NUM_OF_COLUMNS * row + col] = sBox[state[AES_NUM_OF_COLUMNS * row + col]];
        }
    }
}

void invSubBytes(uint8_t *state) {

    for (uint8_t row = 0; row < 4; row++) {
        for (uint8_t col= 0; col < AES_NUM_OF_COLUMNS; col++) {
//            state[numColumns*row+col] = getInvSBoxValue(state[numColumns*row+col]);
            state[AES_NUM_OF_COLUMNS*row+col] = invSBox[state[AES_NUM_OF_COLUMNS*row+col]];
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

uint8_t *initializeAES() {
    return malloc(AES_NUM_OF_COLUMNS * (AES_NUM_OF_ROUNDS + 1) * 4);
}

void aesEncrypt(uint8_t *inputBlock, uint8_t *outputBlock, uint8_t *roundKeys) {

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

void aesDecrypt(uint8_t *inputBlock, uint8_t *outputBlock, uint8_t *roundKeys) {

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