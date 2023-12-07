#include "aes_mpi.h"
#include <mpi.h>

#define BLOCK_SIZE 16

void initAES(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
}

void finalizeAES() {
    MPI_Finalize();
}

void aesEncrypt(uint8_t *inputData, uint8_t *outputData, uint8_t *roundKeys) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    uint8_t localInputBlock[BLOCK_SIZE];
    uint8_t localOutputBlock[BLOCK_SIZE];

    MPI_Scatter(inputData, BLOCK_SIZE, MPI_UINT8_T, localInputBlock, BLOCK_SIZE, MPI_UINT8_T, 0, MPI_COMM_WORLD);

    aesEncrypt(localInputBlock, localOutputBlock, roundKeys);

    MPI_Gather(localOutputBlock, BLOCK_SIZE, MPI_UINT8_T, outputData, BLOCK_SIZE, MPI_UINT8_T, 0, MPI_COMM_WORLD);
}

void aesDecrypt(uint8_t *inputData, uint8_t *outputData, uint8_t *roundKeys) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    uint8_t localInputBlock[BLOCK_SIZE];
    uint8_t localOutputBlock[BLOCK_SIZE];

    MPI_Scatter(inputData, BLOCK_SIZE, MPI_UINT8_T, localInputBlock, BLOCK_SIZE, MPI_UINT8_T, 0, MPI_COMM_WORLD);

    aesDecrypt(localInputBlock, localOutputBlock, roundKeys);

    MPI_Gather(localOutputBlock, BLOCK_SIZE, MPI_UINT8_T, outputData, BLOCK_SIZE, MPI_UINT8_T, 0, MPI_COMM_WORLD);
}
