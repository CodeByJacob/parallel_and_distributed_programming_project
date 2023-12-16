#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>

#include "lib/timer/timer.h"
#include "lib/test_helper/test_helper.h"

#include "aes/aes_common.h"

static const int TEST_NAME_SIZE = 50;


void test_aes(char *test_category, uint8_t *original_block, size_t blocks, uint8_t *key, size_t size);

void init_test_names(size_t size, char *keyExpansion_test_name, size_t keyExpansion_size, char *encrypt_test_name,
                     size_t encrypt_size, char *decrypt_test_name,
                     size_t decrypt_size);

TestCase tests[] = {
        {"./files/extreme_test.txt","./files/key_256_2.txt"},
        {"./files/test2.txt","./files/key_256_3.txt"},
        {"./files/test5.txt","./files/key_256_1.txt"}
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Bad arguments size. It should be 2, containing test category");
        return 1;
    }

    initAES(argc, argv);

    char *test_category = argv[1];

    short testSize = sizeof(tests)/sizeof(TestCase);
    for(uint8_t i = 0; i < testSize; i++) {
        FileData msg = readFromFile(tests[i].msg_path);
        FileData key = readFromFile(tests[i].key_path);

        ConvertedData key_hex = convertDataToUint8(key.content);
        ConvertedData msg_hex = convertDataToUint8(msg.content);

        test_aes(test_category, msg_hex.data, msg_hex.size, key_hex.data, AES_KEYSIZE);

        free(msg.content);
        free(key.content);
        free(key_hex.data);
        free(msg_hex.data);
    }

    finalizeAES();

    return 0;
}


void test_aes(char *test_category, uint8_t *original_block, size_t blocks, uint8_t *key, size_t size) {
    uint8_t *encrypted_block = malloc(blocks);
    uint8_t *decrypted_block = malloc(blocks);

    char encrypt_test_name[TEST_NAME_SIZE], decrypt_test_name[TEST_NAME_SIZE], keyExpansion_test_name[TEST_NAME_SIZE];

    init_test_names(size,
                    keyExpansion_test_name, TEST_NAME_SIZE,
                    encrypt_test_name, TEST_NAME_SIZE,
                    decrypt_test_name, TEST_NAME_SIZE);

    uint8_t *expandedKey = initializeAES();

    TimerData keyExpansion_td = init_time(test_category, "KeyExpansion", size*8);
    keyExpansion(key, expandedKey);
    printTime(&keyExpansion_td);

    TimerData encrypt_td = init_time(test_category, encrypt_test_name,size*8);
    aesEncrypt(original_block, blocks, encrypted_block, expandedKey);
    printTime(&encrypt_td);

    TimerData decrypt_td = init_time(test_category, decrypt_test_name,size*8);
    aesDecrypt(encrypted_block, blocks, decrypted_block, expandedKey);
    printTime(&decrypt_td);

    assert(memcmp(original_block, decrypted_block, blocks) == 0);
}

void init_test_names(size_t size, char *keyExpansion_test_name, size_t keyExpansion_size, char *encrypt_test_name,
                     size_t encrypt_size, char *decrypt_test_name,
                     size_t decrypt_size) {
    snprintf(keyExpansion_test_name, keyExpansion_size*8, "KeyExpansion size=%zu", size);
    snprintf(encrypt_test_name, encrypt_size*8, "Encrypt size=%zu", size);
    snprintf(decrypt_test_name, decrypt_size*8, "Decrypt size=%zu", size);
}