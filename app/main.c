#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "lib/timer/timer.h"

#include "aes/aes_common.h"

static const int TEST_NAME_SIZE = 50;

void test_aes(char *test_category, uint8_t *original_block, uint8_t *key, size_t size);

void init_test_names(size_t size, char *keyExpansion_test_name, size_t keyExpansion_size, char *encrypt_test_name,
                     size_t encrypt_size, char *decrypt_test_name,
                     size_t decrypt_size);

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
        {
                (uint8_t[]) {
                        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,},
                (uint8_t[]) {
                        0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52,
                        0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
                        0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b
                },
                256},

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
    char encrypt_test_name[TEST_NAME_SIZE], decrypt_test_name[TEST_NAME_SIZE], keyExpansion_test_name[TEST_NAME_SIZE];

    init_test_names(size,
                    keyExpansion_test_name, TEST_NAME_SIZE,
                    encrypt_test_name, TEST_NAME_SIZE,
                    decrypt_test_name, TEST_NAME_SIZE);

    uint8_t *expandedKey = initializeAES(size);

    TimerData keyExpansion_td = init_time(test_category, keyExpansion_test_name);
    keyExpansion(key, expandedKey, size);
    printTime(&keyExpansion_td);

    memcpy(encrypted_block, original_block, size);

    TimerData encrypt_td = init_time(test_category, encrypt_test_name);
    aesEncrypt(original_block /* in */, encrypted_block /* out */, expandedKey /* expanded key */, size);
    printTime(&encrypt_td);

    memcpy(decrypted_block, encrypted_block, size);

    TimerData decrypt_td = init_time(test_category, decrypt_test_name);
    aesDecrypt(encrypted_block, decrypted_block, expandedKey, size);
    printTime(&decrypt_td);

    assert(memcmp(original_block, decrypted_block, size) == 0);

    free(expandedKey);
}

void init_test_names(size_t size, char *keyExpansion_test_name, size_t keyExpansion_size, char *encrypt_test_name,
                     size_t encrypt_size, char *decrypt_test_name,
                     size_t decrypt_size) {
    snprintf(keyExpansion_test_name, keyExpansion_size, "KeyExpansion size=%zu", size);
    snprintf(encrypt_test_name, encrypt_size, "Encrypt size=%zu", size);
    snprintf(decrypt_test_name, decrypt_size, "Decrypt size=%zu", size);
}