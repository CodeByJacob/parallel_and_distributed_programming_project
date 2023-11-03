#include "gmult.h"

uint8_t generalized_multiplication(uint8_t multiplicand, uint8_t multiplier) {
    uint8_t product = 0x00;

    for (; multiplicand && multiplier; multiplier >>= 1) {
        if (multiplier & 1)
            product ^= multiplicand;
        if (multiplicand & 0x80)
            multiplicand = (multiplicand << 1) ^ 0x1B;
        else
            multiplicand <<=1;
    }
    return product;
}

