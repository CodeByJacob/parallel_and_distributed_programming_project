#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "lib/timer/timer.h"

#include "aes/aes_common.h"

static const int TEST_NAME_SIZE = 50;

void test_aes(char *test_category, uint8_t *original_block, uint8_t *key, size_t size);

struct {
    uint8_t *original_block;
    uint8_t *key;
    size_t size;
} tests[] = {
        {
                (uint8_t[]) {
                        0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a, 0x30, 0x8d,
                        0x31, 0x31, 0x98, 0xa2, 0xe0, 0x37, 0x07, 0x34},
                (uint8_t[]) {
                        0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52,
                        0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
                        0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b
                        },
                64},

};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Bad arguments size. It should be 2, containing test category");
        return 1;
    }

    initAES(argc, argv);

    char *test_category = argv[1];

    uint8_t tests_size = sizeof(tests) / sizeof(tests[0]);

    for (uint8_t i = 0; i < tests_size; i++) {
        test_aes(test_category, tests[i].original_block, tests[i].key, tests[i].size);
    }

    finalizeAES();

    return 0;
}

void test_aes(char *test_category, uint8_t *original_block, uint8_t *key, size_t size) {
    uint8_t encrypted_block[size];
    uint8_t decrypted_block[size];

    uint8_t *expandedKey = initializeAES();

    TimerData keyExpansion_td = init_time(test_category, "KeyExpansion", size);
    keyExpansion(key, expandedKey);
    printTime(&keyExpansion_td);

    memcpy(encrypted_block, original_block, size);

    TimerData encrypt_td = init_time(test_category, "Encrypt", size);
    aesEncrypt(original_block /* in */, encrypted_block /* out */, expandedKey /* expanded key */, size);
    printTime(&encrypt_td);

    memcpy(decrypted_block, encrypted_block, size);

    TimerData decrypt_td = init_time(test_category, "Decrypt", size);
    aesDecrypt(encrypted_block, decrypted_block, expandedKey, size);
    printTime(&decrypt_td);

    assert(memcmp(original_block, decrypted_block, size) == 0);

    free(expandedKey);
}