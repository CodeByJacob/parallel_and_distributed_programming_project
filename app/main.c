#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>

#include "lib/timer/timer.h"
#include "lib/test_helper/test_helper.h"

#include "aes/aes_common.h"
#include "aes/aes_sequential.h"

static const int TEST_NAME_SIZE = 50;

void test_aes_sequential(char *test_category, uint8_t *original_block, size_t blocks, uint8_t *key, size_t size);

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
                        0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b},
                AES_KEYSIZE},
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Bad arguments size. It should be 2, containing test category");
        return 1;
    }

    char *test_category = argv[1];

    FileData dataTest1 = readFromFile("./files/test1.txt");

    char *key = generateRandomKey(AES_KEYSIZE * 8);

    ConvertedData key_hex = convertDataToUint8(key);
    ConvertedData msg = convertDataToUint8(dataTest1.content);

    printf("KEY: %s\n",key_hex.data);
    printf("O_MSG: ");
    for (uint8_t i = 0; i < msg.size; i++) {
        printf("%c", msg.data[i]);
    }
    printf("\n");

    test_aes_sequential(test_category, msg.data, msg.size, key_hex.data, AES_KEYSIZE);

//
//    uint8_t tests_size = sizeof(tests)/sizeof(tests[0]);
//
//    for(uint8_t i = 0; i < tests_size; i++) {
//        test_aes_sequential(test_category, tests[i].original_block, tests[i].key, AES_KEYSIZE);
//    }
//

//    freeBlocks(msg,blocks);
//
//    for (uint8_t i = 0; i < blocks; i++) {
//        freeUint8Array(msg_hex[i]);
//    }
//    free(msg_hex);

    return 0;
}

void test_aes_sequential(char *test_category, uint8_t *original_block, size_t blocks, uint8_t *key, size_t size) {
    uint8_t *encrypted_block = malloc(blocks);
    uint8_t *decrypted_block = malloc(blocks);

    Block msg[(blocks/BLOCK_SIZE) + 1];
    EncryptedBlock e_msg[(blocks/BLOCK_SIZE) + 1];
    DecryptedBlock d_msg[(blocks/BLOCK_SIZE) + 1];

    for (int i = 0; i < ((blocks / BLOCK_SIZE) + 1); i++) {
        for (int j = 0; j < BLOCK_SIZE; j++) {
            msg[i].data[j] = original_block[(i * BLOCK_SIZE) + j];
            printf("%c[%02X] ", msg[i].data[j],msg[i].data[j]);
        }
    }
    printf("\n");

    char encrypt_test_name[TEST_NAME_SIZE], decrypt_test_name[TEST_NAME_SIZE], keyExpansion_test_name[TEST_NAME_SIZE];

    init_test_names(size,
                    keyExpansion_test_name, TEST_NAME_SIZE,
                    encrypt_test_name, TEST_NAME_SIZE,
                    decrypt_test_name, TEST_NAME_SIZE);

    uint8_t *expandedKey = initializeAES();

    TimerData keyExpansion_td = init_time(test_category, keyExpansion_test_name);
    keyExpansion(key, expandedKey);
    printTime(&keyExpansion_td);

//    memcpy(encrypted_block, original_block, size);

    TimerData encrypt_td = init_time(test_category, encrypt_test_name);
    for(int i = 0; i < ((blocks / BLOCK_SIZE) + 1) ; i++) {
        aesEncrypt(msg[i].data /* in */, e_msg[i].data /* out */, expandedKey /* expanded key */);
    }
     printTime(&encrypt_td);

    for(int i = 0; i < ((blocks / BLOCK_SIZE) + 1) ; i++) {
        for(int j =0; j < BLOCK_SIZE; j++){
            encrypted_block[(i*BLOCK_SIZE) + j] = e_msg[i].data[j];
        }
    }

//    memcpy(decrypted_block, encrypted_block, size);

    TimerData decrypt_td = init_time(test_category, decrypt_test_name);
    for(int i = 0; i < ((blocks / BLOCK_SIZE) + 1) ; i++) {
        aesDecrypt(e_msg[i].data, d_msg[i].data, expandedKey);
    }
    printTime(&decrypt_td);

    for(int i = 0; i < ((blocks / BLOCK_SIZE) + 1) ; i++) {
        for(int j =0; j < BLOCK_SIZE ; j++){
            decrypted_block[(i*size) + j] = d_msg[i].data[j];
            printf("%c[%02X] ", decrypted_block[(i*size) + j],decrypted_block[(i*size) + j]);
//            printf("%c", decrypted_block[(i*size) + j]);
        }
    }
    printf("\n");

    printf("D_MSG: ");
    for(int i = 0; i < ((blocks / BLOCK_SIZE) + 1) ; i++) {
        for(int j =0; j < BLOCK_SIZE ; j++){
            printf("%c", decrypted_block[(i*size) + j]);
        }
    }
    printf("\n");

    int result = memcmp(original_block, decrypted_block, blocks);

    // Check the result
    if (result == 0) {
        printf("Arrays are equal.\n");
    } else {
        printf("Arrays are not equal. The first differing byte is at position %d.\n", result);
        printf("ob: %c |db: %c \n", original_block[result], decrypted_block[result]);
    }
//    assert(memcpy(original_block, decrypted_block, blocks));

    free(expandedKey);
}


void init_test_names(size_t size, char *keyExpansion_test_name, size_t keyExpansion_size, char *encrypt_test_name,
                     size_t encrypt_size, char *decrypt_test_name,
                     size_t decrypt_size) {
    snprintf(keyExpansion_test_name, keyExpansion_size, "KeyExpansion size=%zu", size);
    snprintf(encrypt_test_name, encrypt_size, "Encrypt size=%zu", size);
    snprintf(decrypt_test_name, decrypt_size, "Decrypt size=%zu", size);
}