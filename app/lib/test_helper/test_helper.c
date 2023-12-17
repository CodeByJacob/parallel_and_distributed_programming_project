#include "test_helper.h"


ConvertedData convertDataToUint8(const char *data) {
    ConvertedData convertedData;
    convertedData.size = strlen(data);
    convertedData.data = (uint8_t *) malloc(convertedData.size * sizeof(uint8_t));

    if (convertedData.data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        convertedData.size = 0;
        convertedData.data = NULL;
        return convertedData;
    }

    memcpy(convertedData.data, data, convertedData.size);

    return convertedData;
}

FileData readFromFile(const char *filename) {
    FileData fileData;
    FILE *filePointer = fopen(filename, "rb");

    if (filePointer == NULL) {
        fprintf(stderr, "Error opening file\n");
        fileData.content = NULL;
        fileData.size = 0;
        return fileData;
    }

    fseek(filePointer, 0, SEEK_END);
    fileData.size = ftell(filePointer);
    fseek(filePointer, 0, SEEK_SET);

    fileData.content = (char *) malloc(fileData.size + 1);  // Allocate extra byte for null terminator

    if (fileData.content == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(filePointer);
        fileData.size = 0;
        return fileData;
    }

    if (fread(fileData.content, 1, fileData.size, filePointer) != fileData.size) {
        fprintf(stderr, "Error reading file\n");
        free(fileData.content);
        fclose(filePointer);
        fileData.size = 0;
        return fileData;
    }

    fileData.content[fileData.size] = '\0';  // Null-terminate the string

    fclose(filePointer);

    return fileData;
}

void shuffle(char *array, size_t n) {
    srand((unsigned int) time(NULL));
    if (n > 1) {
        for (size_t i = n - 1; i > 0; --i) {
            size_t j = rand() % (i + 1);
            char temp = array[i];
            array[i] = array[j];
            array[j] = temp;
        }
    }
}

char *generateRandomKey(size_t keyLengthInBits) {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    size_t charsetLength = sizeof(charset) - 1;

    char *shuffledCharset = malloc(sizeof(charset));
    if (shuffledCharset == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    strcpy(shuffledCharset, charset);
    shuffle(shuffledCharset, charsetLength);

    size_t keyLengthInBytes = (keyLengthInBits + 7) / 8;

    char *key = (char *) malloc((keyLengthInBytes + 1) * sizeof(char));

    if (key == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < keyLengthInBytes; ++i) {
        key[i] = shuffledCharset[i % charsetLength];
    }

    key[keyLengthInBytes] = '\0';

    free(shuffledCharset);

    return key;
}