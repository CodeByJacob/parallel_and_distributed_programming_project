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

uint8_t *initializeAES() {
    return malloc(AES_NUM_OF_COLUMNS * (AES_NUM_OF_ROUNDS + 1) * 4);
}