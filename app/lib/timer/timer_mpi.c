#include <stdio.h>
#include <mpi.h>
#include "timer.h"

// Initialize the timer for MPI code
TimerData init_time(char* testCategory, char* testName, int testSize) {
    TimerData timerData;
    timerData.clockTime = MPI_Wtime();
    timerData.testCategory = testCategory;
    timerData.testName = testName;
    timerData.testSize = testSize;
    return timerData;
}

// Calculate elapsed clock time in seconds for MPI code
double calculateTimeClockInSecond(TimerData *timerData) {
    return MPI_Wtime() - timerData->clockTime;
}

// Print the timing information for MPI code
void printTime(TimerData *timerData) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank == 0) {
        double elapsed = calculateTimeClockInSecond(timerData);
        printf("TestCategory:%s|TestName:%s|Size:%d|ClockTime:%lf\n", timerData->testCategory, timerData->testName, timerData->testSize, elapsed);
    }
}
