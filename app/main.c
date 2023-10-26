#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "aes/aes_common.h"
#include "lib/timer/timer.h"

void test_aes_sequential(char *test_category, uint8_t *original_block, uint8_t *key, size_t size);

void init_test_names(size_t size, char *encrypt_test_name, char *decrypt_test_name);

struct {
    uint8_t *original_block;
    uint8_t *key;
    size_t size;
} tests[] = {
        {"Mock block", "Mock key", 11}
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Bad arguments size. It should be 2, containing test category");
        return 1;
    }

    char *test_category = argv[1];
    int tests_size = sizeof(tests) / sizeof(tests[0]);

    AES_init();

    for (int i = 0; i < tests_size; i++) {
        test_aes_sequential(test_category, tests[i].original_block, tests[i].key, tests[i].size);
    }

    return 0;
}

void test_aes_sequential(char *test_category, uint8_t *original_block, uint8_t *key, size_t size) {
    uint8_t encrypted_block[size];
    uint8_t decrypted_block[size];
    char encrypt_test_name[50], decrypt_test_name[50];

    init_test_names(size, encrypt_test_name, decrypt_test_name);

    memcpy(encrypted_block, original_block, size);

    TimerData encrypt_td = init_time(test_category, encrypt_test_name);
    AES_encrypt(encrypted_block, key);
    printTime(&encrypt_td);

    memcpy(decrypted_block, encrypted_block, size);

    TimerData decrypt_td = init_time(test_category, decrypt_test_name);
    AES_decrypt(decrypted_block, key);
    printTime(&decrypt_td);

    assert(memcmp(original_block, decrypted_block, size) == 0);
}

void init_test_names(size_t size, char *encrypt_test_name, char *decrypt_test_name) {
    snprintf(encrypt_test_name, sizeof(encrypt_test_name), "Encrypt size: %zu", size);
    snprintf(decrypt_test_name, sizeof(decrypt_test_name), "Decrypt size: %zu", size);
}

