#include <stdio.h>

#include "lib/timer/timer.h"
#include "lib/test_helper/test_helper.h"

#include "aes/aes_common.h"

void test_aes(char *test_category, uint8_t *original_block, size_t blocks, uint8_t *key);

TestCase tests[] = {
        {"./files/test2.txt","./files/key_256_3.txt", 147},
        {"./files/test6.txt","./files/key_256_1.txt", 699},
        {"./files/extreme_test.txt","./files/key_256_2.txt", 1391},
};

int main(int argc, char *argv[]) {
    initAES(argc, argv);

    if (argc != 2) {
        printf("Bad arguments size. It should be 2, containing test category");
        finalizeAES();
        return 1;
    }

    char *test_category = argv[1];

    short testSize = sizeof(tests)/sizeof(TestCase);
    for(short i = 0; i < testSize; i++) {
        FileData msg = readFromFile(tests[i].msg_path);
        FileData key = readFromFile(tests[i].key_path);

        ConvertedData key_hex = convertDataToUint8(key.content);
        ConvertedData msg_hex = convertDataToUint8(msg.content);

        test_aes(test_category, msg_hex.data, tests[i].msg_size, key_hex.data);

        free(msg.content);
        free(key.content);
        free(key_hex.data);
        free(msg_hex.data);
    }

    finalizeAES();

    return 0;
}

void test_aes(char *test_category, uint8_t *original_block, size_t blocks, uint8_t *key) {
    size_t blocks_malloc = (blocks + BLOCK_SIZE - 1) / BLOCK_SIZE * BLOCK_SIZE; // Round up to the nearest multiple of BLOCK_SIZE
    uint8_t *encrypted_block = malloc(blocks_malloc);
    uint8_t *decrypted_block = malloc(blocks_malloc);

    uint8_t *expandedKey = initializeAES();

    TimerData keyExpansion_td = init_time(test_category, "KeyExpansion", blocks);
    keyExpansion(key, expandedKey);
    printTime(&keyExpansion_td);

    TimerData encrypt_td = init_time(test_category, "Encrypt",blocks);
    aesEncrypt(original_block, blocks, encrypted_block, expandedKey);
    printTime(&encrypt_td);

    TimerData decrypt_td = init_time(test_category, "Decrypt",blocks);
    aesDecrypt(encrypted_block, blocks, decrypted_block, expandedKey);
    printTime(&decrypt_td);

    testOriginalAndDecryptedBlock(original_block, decrypted_block, blocks);

    free(expandedKey);
    free(encrypted_block);
    free(decrypted_block);
}
